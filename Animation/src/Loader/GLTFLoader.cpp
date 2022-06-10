#include <Loader/GLTFLoader.h>

#include <iostream>

#include <Math/Matrix4.h>
#include <Math/Transform.h>

#include <Animation/AnimationKeyFrame.h>
#include <Animation/AnimationPose.h>
#include <Animation/AnimationTrack.h>

#include <spdlog/spdlog.h>

using namespace Math;

namespace GLTFHelpers
{
	Transform getLocalTransform(cgltf_node& node)
	{
		Transform result;
		
		if (node.has_matrix)
		{
			Matrix4 matrix(&node.matrix[0]);
			result = matrix4ToTransform(matrix);
		}

		if (node.has_translation)
		{
			result.position = Vector3(node.translation[0], node.translation[1], node.translation[2]);
		}

		if (node.has_rotation)
		{
			result.rotation = Quaternion(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
		}

		if (node.has_scale)
		{
			result.scale = Vector3(node.scale[0], node.scale[1], node.scale[2]);
		}

		return result;
	}

	int32_t getNodeIndex(cgltf_node* target, cgltf_node* allNodes, uint32_t numNodes)
	{
		if (target == nullptr)
		{
			return -1;
		}

		for (uint32_t i = 0; i < numNodes; i++)
		{
			if (target == &allNodes[i])
			{
				return static_cast<int32_t>(i);
			}
		}

		return -1;
	}

	void getScalarValues(std::vector<float>& outScalars, uint32_t componentCount, const cgltf_accessor& inAccessor)
	{
		outScalars.resize(inAccessor.count * componentCount);

		for (cgltf_size i = 0; i < inAccessor.count; i++)
		{
			cgltf_accessor_read_float(&inAccessor, i, &outScalars[i * componentCount], componentCount);
		}
	}

	template <typename T, int32_t N>
	void animationTrackFromChannel(AnimationTrack<T, N>& result, const cgltf_animation_channel& channel)
	{
		cgltf_animation_sampler& sampler = *channel.sampler;

		Interpolation interpolation = Interpolation::Constant;

		if (sampler.interpolation == cgltf_interpolation_type_linear)
		{
			interpolation = Interpolation::Linear;
		}
		else if (sampler.interpolation == cgltf_interpolation_type_cubic_spline)
		{
			interpolation = Interpolation::Cubic;
		}

		bool bIsSamplerCubic = interpolation == Interpolation::Cubic;

		result.setInterpolation(interpolation);

		// Time line elements(frames)
		std::vector<float> times;
		getScalarValues(times, 1, *sampler.input);

		// Animation values
		std::vector<float> values;
		getScalarValues(values, N, *sampler.output);

		uint32_t numFrames = static_cast<uint32_t>(sampler.input->count);
		uint32_t componentCount = static_cast<uint32_t>(values.size() / times.size());
		result.resize(numFrames);

		for (uint32_t i = 0; i < numFrames; i++)
		{
			int32_t baseIndex = static_cast<int32_t>(i * componentCount);
			AnimationKeyFrame<N>& keyframe = result[i];
			int32_t offset = 0;

			keyframe.time = times[i];

			for (int32_t component = 0; component < N; component++)
			{
				keyframe.in[component] = bIsSamplerCubic ? values[baseIndex + offset++] : 0.0f;
			}

			for (int32_t component = 0; component < N; component++)
			{
				keyframe.value[component] = values[baseIndex + offset++];
			}

			for (int32_t component = 0; component < N; component++)
			{
				keyframe.out[component] = bIsSamplerCubic ? values[baseIndex + offset++] : 0.0f;
			}
		}
	}
}

namespace Loader
{
	cgltf_data* loadGLTFFile(const std::string& path)
	{
		cgltf_options options = {0};

		cgltf_data* data = nullptr;
		
		cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
		
		if (result != cgltf_result_success)
		{
			spdlog::info("Failed to parse glTF file: {0}\n");
			freeGLTFFile(data);
			return nullptr;
		}
		
		result = cgltf_load_buffers(&options, data, path.c_str());
		
		if (result != cgltf_result_success)
		{
			cgltf_free(data);
			spdlog::info("Could not load buffers for: {0}\n", path);
			return 0;
		}

		result = cgltf_validate(data);

		if (result != cgltf_result_success)
		{
			spdlog::info("Failed to validate glTF file: {0}\n", path);
			freeGLTFFile(data);
			return nullptr;
		}
		
		return data;
	}

	void freeGLTFFile(cgltf_data* data)
	{
		cgltf_free(data);
	}

	AnimationPose loadRestPose(cgltf_data* data)
	{
		uint32_t boneCount = static_cast<uint32_t>(data->nodes_count);

		AnimationPose result(boneCount);

		for (uint32_t i = 0; i < boneCount; i++)
		{
			cgltf_node* node = &(data->nodes[i]);

			Transform localTransform = GLTFHelpers::getLocalTransform(*node);
			result.setLocalTransform(i, localTransform);

			int32_t parentId = GLTFHelpers::getNodeIndex(node->parent, data->nodes, boneCount);
			result.setParent(i, parentId);
		}

		return result;
	}
	
	std::vector<std::string> loadJointNames(cgltf_data* data)
	{
		uint32_t boneCount = static_cast<uint32_t>(data->nodes_count);
		std::vector<std::string> result(boneCount, "");

		for (uint32_t i = 0; i < boneCount; i++)
		{
			cgltf_node* node = &(data->nodes[i]);
			
			if (node->name == nullptr)
			{
				result[i] = "EmptyNode";
			}
			else
			{
				result[i] = node->name;
			}
		}

		return result;
	}

	std::vector<AnimationClip> loadAnimationClips(cgltf_data* data)
	{
		uint32_t numClips = static_cast<uint32_t>(data->animations_count);
		uint32_t numNodes = static_cast<uint32_t>(data->nodes_count);

		std::vector<AnimationClip> result;
		result.resize(numClips);

		for (uint32_t i = 0; i < numClips; i++)
		{
			result[i].setName(data->animations[i].name);

			uint32_t numChannels = static_cast<uint32_t>(data->animations[i].channels_count);

			for (uint32_t j = 0; j < numChannels; j++)
			{
				cgltf_animation_channel& channel = data->animations[i].channels[j];

				cgltf_node* target = channel.target_node;
				int32_t nodeId = GLTFHelpers::getNodeIndex(target, data->nodes, numNodes);

				if (channel.target_path == cgltf_animation_path_type_scale)
				{
					VectorTrack& track = result[i][nodeId].getScaleTrack();
					GLTFHelpers::animationTrackFromChannel<Vector3, 3>(track, channel);
				}

				if (channel.target_path == cgltf_animation_path_type_rotation)
				{
					QuaternionTrack& track = result[i][nodeId].getRotationTrack();
					GLTFHelpers::animationTrackFromChannel<Quaternion, 4>(track, channel);
				}

				if (channel.target_path == cgltf_animation_path_type_translation)
				{
					VectorTrack& track = result[i][nodeId].getPositionTrack();
					GLTFHelpers::animationTrackFromChannel<Vector3, 3>(track, channel);
				}
			}
			result[i].recalculateDuration();
		}
		return result;
	}
}

