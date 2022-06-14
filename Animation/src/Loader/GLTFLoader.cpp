#include <Loader/GLTFLoader.h>

#include <iostream>

#include <Math/Matrix4.h>
#include <Math/Transform.h>

#include <Animation/AnimationKeyFrame.h>
#include <Animation/AnimationPose.h>
#include <Animation/AnimationTrack.h>
#include <Animation/SkeletalMesh.h>

#include <Math/Math.h>

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

	void meshFromAttribute(SkeletalMesh& outMesh, cgltf_attribute& attribute, cgltf_skin* skin, cgltf_node* nodes, uint32_t nodeCount)
	{
		cgltf_attribute_type attributeType = attribute.type;
		cgltf_accessor& accessor = *attribute.data;

		uint32_t componentCount = 0;
		
		if (accessor.type == cgltf_type_vec2)
		{
			componentCount = 2;
		}
		else if (accessor.type == cgltf_type_vec3)
		{
			componentCount = 3;
		}
		else if (accessor.type == cgltf_type_vec4)
		{
			componentCount = 4;
		}

		std::vector<float> scalars;
		getScalarValues(scalars, componentCount, accessor);
		uint32_t accessorCount = static_cast<uint32_t>(accessor.count);

		std::vector<Vector3>& positions = outMesh.getPositions();
		std::vector<Vector3>& normals = outMesh.getNormals();
		std::vector<Vector3>& tangents = outMesh.getTangents();
		std::vector<Vector2>& texcoords = outMesh.getTexcoords();
		std::vector<Vector4i>& influenceJoints = outMesh.getInfluenceJoints();
		std::vector<Vector4>& weights = outMesh.getWeights();

		for (uint32_t i = 0; i < accessorCount; i++)
		{
			int32_t index = i * componentCount;
			
			switch (attributeType)
			{
				case cgltf_attribute_type_position:
				positions.emplace_back(Vector3(scalars[index + 0], scalars[index + 1], scalars[index + 2]));
				break;
				case cgltf_attribute_type_normal:
				{
					Vector3 normal = Vector3(scalars[index + 0], scalars[index + 1], scalars[index + 2]);

					if (lengthSqure(normal) < Math::Epsilon)
					{
						normal = Vector3(0.0f, 1.0f, 0.0f);
					}

					normals.emplace_back(normal);
				}
				break;
				case cgltf_attribute_type_tangent:
					tangents.emplace_back(Vector3(scalars[index + 0], scalars[index + 1], scalars[index + 2]));
				break;
				case cgltf_attribute_type_texcoord:
					texcoords.emplace_back(Vector2(scalars[index + 0], scalars[index + 1]));
				break;
				case cgltf_attribute_type_color:
				break;
				case cgltf_attribute_type_joints:
				{
					// These indices are skin relative
					// This function has no information about the
					// skin that is being parsed. Add +0.5f to round,
					// since we can't read integers
					Vector4i joints = Vector4i(static_cast<int32_t>(scalars[index + 0] + 0.5f),
											   static_cast<int32_t>(scalars[index + 1] + 0.5f),
											   static_cast<int32_t>(scalars[index + 2] + 0.5f),
											   static_cast<int32_t>(scalars[index + 3] + 0.5f));

					joints.x = getNodeIndex(skin->joints[joints.x], nodes, nodeCount);
					joints.y = getNodeIndex(skin->joints[joints.y], nodes, nodeCount);
					joints.z = getNodeIndex(skin->joints[joints.z], nodes, nodeCount);
					joints.w = getNodeIndex(skin->joints[joints.w], nodes, nodeCount);

					// Make sure that event the invalid nodes have a value of 0
					// Any negative joint indices will break the skinning implementation
					joints.x = Math::Max(0, joints.x);
					joints.y = Math::Max(0, joints.y);
					joints.z = Math::Max(0, joints.z);
					joints.w = Math::Max(0, joints.w);
					
					influenceJoints.emplace_back(joints);
				}
				break;
			case cgltf_attribute_type_weights:
				weights.emplace_back(Vector4(scalars[index + 0], scalars[index + 1], scalars[index + 2], scalars[index + 3]));
				break;
			default:
				break;
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

		AnimationPose restPose(boneCount);

		for (uint32_t i = 0; i < boneCount; i++)
		{
			cgltf_node* node = &(data->nodes[i]);

			Transform localTransform = GLTFHelpers::getLocalTransform(*node);
			restPose.setLocalTransform(i, localTransform);

			int32_t parentId = GLTFHelpers::getNodeIndex(node->parent, data->nodes, boneCount);
			restPose.setParent(i, parentId);
		}

		return restPose;
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

	AnimationPose loadBindPose(cgltf_data* data)
	{
		AnimationPose restPose = loadRestPose(data);

		uint32_t numBones = restPose.getSize();
		std::vector<Transform> worldBindPose(numBones);
		for (uint32_t i = 0; i < numBones; i++)
		{			
			worldBindPose[i] = restPose.getGlobalTransform(i);
		}

		uint32_t numSkins = static_cast<uint32_t>(data->skins_count);

		for (uint32_t i = 0; i < numSkins; i++)
		{
			cgltf_skin* skin = &(data->skins[i]);
			std::vector<float> inverseBindMatrices;

			GLTFHelpers::getScalarValues(inverseBindMatrices, 16, *skin->inverse_bind_matrices);

			uint32_t numJoints = static_cast<uint32_t>(skin->joints_count);

			for (uint32_t j = 0; j < numJoints; j++)
			{
				// Read the inverse bind matrix of the joint
				float* values = &(inverseBindMatrices[j * 16]);
				Matrix4 inverseBindMatrix = Matrix4(values);

				// Invert, convert to transform
				Matrix4 bindMatrix = inverse(inverseBindMatrix);
				Transform bindTransform = matrix4ToTransform(bindMatrix);

				// Set that transform in the worldBindPose
				cgltf_node* jointNode = skin->joints[j];
				int32_t jointIndex = GLTFHelpers::getNodeIndex(jointNode, data->nodes, numBones);

				worldBindPose[jointIndex] = bindTransform;
			}
		}

		// Convert the world bind pose to a regular bind pose
		// Covert each joint so that it is relative to its parent.
		// To move a joint into the space of another joint,
		// that is, to make it relative to another joint, combine
		// the world transform of the joint with the inverse world
		// transform of its parent
		AnimationPose bindPose = restPose;

		for (uint32_t i = 0; i < numBones; i++)
		{
			Transform currentBindPose = worldBindPose[i];
			int32_t parentId = bindPose.getParent(i);

			if (parentId >= 0)
			{
				// Bring into parent space
				Transform parentBindPose = worldBindPose[parentId];
				currentBindPose = combine(inverse(parentBindPose), currentBindPose);
			}
			bindPose.setLocalTransform(i, currentBindPose);
		}

		return bindPose;
	}

	Skeleton loadSkeleton(cgltf_data* data)
	{
		return Skeleton(loadRestPose(data), loadBindPose(data), loadJointNames(data));
	}

	std::vector<Animation::SkeletalMesh> loadMeshes(cgltf_data* data)
	{
		std::vector<SkeletalMesh> result;
		cgltf_node* nodes = data->nodes;

		uint32_t nodeCount = static_cast<uint32_t>(data->nodes_count);

		for (uint32_t i = 0; i < nodeCount; i++)
		{
			cgltf_node* node = &(nodes[i]);

			if (node->mesh == nullptr)
			{
				continue;
			}

			bool bHasAnimation = true;
			
			if (node->skin == nullptr)
			{
				bHasAnimation = false;
			}

			int32_t numPrimitives = static_cast<int32_t>(node->mesh->primitives_count);
			
			for (int32_t j = 0; j < numPrimitives; j++)
			{
				SkeletalMesh mesh;

				mesh.hasAnimation() = bHasAnimation;

				cgltf_primitive* primitive = &(node->mesh->primitives[j]);

				uint32_t attributesCount = static_cast<uint32_t>(primitive->attributes_count);

				for (uint32_t k = 0; k < attributesCount; k++)
				{
					cgltf_attribute* attribute = &(primitive->attributes[k]);
					GLTFHelpers::meshFromAttribute(mesh, *attribute, node->skin, nodes, nodeCount);
				}

				// Check whether the primitive contains indices
				if (primitive->indices != nullptr)
				{
					uint32_t indicesCount = static_cast<uint32_t>(primitive->indices->count);
					std::vector<uint32_t>& indices = mesh.getIndices();
					indices.resize(indicesCount);

					for (uint32_t k = 0; k < indicesCount; k++)
					{
						indices[k] = static_cast<uint32_t>(cgltf_accessor_read_index(primitive->indices, k));
					}
				}
				mesh.updateOpenGLBuffers();
				result.emplace_back(mesh);
			}
		}
		return result;
	}
}

