#include "SkeletalMesh.h"

#include <Renderer/Renderer.h>
#include <Utils/Timer.h>

namespace Animation
{
	SkeletalMesh::SkeletalMesh()
	{
		positionsAttribute = std::make_shared<Attribute<Vector3>>();
		normalsAttribute = std::make_shared<Attribute<Vector3>>();
		texcoordsAttribute = std::make_shared<Attribute<Vector2>>();
		weightsAttribute = std::make_shared<Attribute<Vector4>>();
		influencesAttribute = std::make_shared<Attribute<Vector4i>>();

		indexBuffer = std::make_shared<IndexBuffer>();
	}

	SkeletalMesh::SkeletalMesh(const SkeletalMesh& mesh)
	{
		positionsAttribute = std::make_shared<Attribute<Vector3>>();
		normalsAttribute = std::make_shared<Attribute<Vector3>>();
		texcoordsAttribute = std::make_shared<Attribute<Vector2>>();
		weightsAttribute = std::make_shared<Attribute<Vector4>>();
		influencesAttribute = std::make_shared<Attribute<Vector4i>>();

		indexBuffer = std::make_shared<IndexBuffer>();

		*this = mesh;
	}

	SkeletalMesh& SkeletalMesh::operator=(const SkeletalMesh& mesh)
	{
		if (&mesh == this)
		{
			return *this;
		}

		positions = mesh.positions;
		normals = mesh.normals;
		texcoords = mesh.texcoords;
		weights = mesh.weights;
		influenceJoints = mesh.influenceJoints;
		indices = mesh.indices;

		updateOpenGLBuffers();

		return *this;
	}

	SkeletalMesh::~SkeletalMesh()
	{

	}

	std::vector<Vector3>& SkeletalMesh::getPositions()
	{
		return positions;
	}

	std::vector<Vector3>& SkeletalMesh::getNormals()
	{
		return normals;
	}

	const std::vector<Math::Vector3>& SkeletalMesh::getTangents() const
	{
		return tangents;
	}

	std::vector<Math::Vector3>& SkeletalMesh::getTangents()
	{
		return tangents;
	}

	std::vector<Vector2>& SkeletalMesh::getTexcoords()
	{
		return texcoords;
	}
	 
	std::vector<Vector4>& SkeletalMesh::getWeights()
	{
		return weights;
	}
	
	std::vector<Vector4i>& SkeletalMesh::getInfluenceJoints()
	{
		return influenceJoints;
	}
	
	std::vector<uint32_t>& SkeletalMesh::getIndices()
	{
		return indices;
	}
	
	const std::vector<Math::Vector3>& SkeletalMesh::getPositions() const
	{
		return positions;
	}

	const std::vector<Math::Vector3>& SkeletalMesh::getNormals() const
	{
		return normals;
	}

	const std::vector<Math::Vector2>& SkeletalMesh::getTexcoords() const
	{
		return texcoords;
	}

	const std::vector<Math::Vector4>& SkeletalMesh::getWeights() const
	{
		return weights;
	}

	const std::vector<Math::Vector4i>& SkeletalMesh::getInfluenceJoints() const
	{
		return influenceJoints;
	}

	const std::vector<uint32_t>& SkeletalMesh::getIndices() const
	{
		return indices;
	}

	void SkeletalMesh::CPUSkinUseTransform(const Skeleton& skeleton, const AnimationPose& animationPose)
	{
		uint32_t numVertices = static_cast<uint32_t>(positions.size());
		
		if (numVertices == 0)
		{
			return;
		}

		skinnedPosition.resize(numVertices);
		skinnedNormal.resize(numVertices);
		
		const AnimationPose& bindPose = skeleton.getBindPose();

		for (uint32_t i = 0; i < numVertices; i++)
		{
			Vector4i& joint = influenceJoints[i];
			Vector4& weight = weights[i];

			Transform skinTransform0 = combine(animationPose[joint.x], inverse(bindPose[joint.x]));
			Vector3 position0 = transformPoint(skinTransform0, positions[i]);
			Vector3 normal0 = transformVector(skinTransform0, normals[i]);

			Transform skinTransform1 = combine(animationPose[joint.y], inverse(bindPose[joint.y]));
			Vector3 position1 = transformPoint(skinTransform1, positions[i]);
			Vector3 normal1 = transformVector(skinTransform1, normals[i]);

			Transform skinTransform2 = combine(animationPose[joint.z], inverse(bindPose[joint.z]));
			Vector3 position2 = transformPoint(skinTransform2, positions[i]);
			Vector3 normal2 = transformVector(skinTransform2, normals[i]);

			Transform skinTransform3 = combine(animationPose[joint.w], inverse(bindPose[joint.w]));
			Vector3 position3 = transformPoint(skinTransform3, positions[i]);
			Vector3 normal3 = transformVector(skinTransform3, normals[i]);

			skinnedPosition[i] = position0 * weight.x + position1 * weight.y + position2 * weight.z + position3 * weight.w;
			skinnedNormal[i] = normal0 * weight.x + normal1 * weight.y + normal2 * weight.z + normal3 * weight.w;
		}

		positionsAttribute->set(skinnedPosition);
		normalsAttribute->set(skinnedNormal);
	}

	void SkeletalMesh::CPUSkinUseMatrixPalette(const Skeleton& skeleton, const AnimationPose& animationPose)
	{
		uint32_t numVertices = static_cast<uint32_t>(positions.size());

		if (numVertices == 0)
		{
			return;
		}

		skinnedPosition.resize(numVertices);
		skinnedNormal.resize(numVertices);
		
		animationPose.getMatrixPalette(animationPosePalette);

		std::vector<Matrix4> inversePosePalette = skeleton.getInverseBindPose();

		for (uint32_t i = 0; i < numVertices; i++)
		{
			Vector4i& jointIds = influenceJoints[i];
			Vector4& weight = weights[i];

			Matrix4 matrix0 = (animationPosePalette[jointIds.x] * inversePosePalette[jointIds.x]) * weight.x;
			Matrix4 matrix1 = (animationPosePalette[jointIds.y] * inversePosePalette[jointIds.y]) * weight.y;
			Matrix4 matrix2 = (animationPosePalette[jointIds.z] * inversePosePalette[jointIds.z]) * weight.z;
			Matrix4 matrix3 = (animationPosePalette[jointIds.w] * inversePosePalette[jointIds.w]) * weight.w;

			Matrix4 skinMatrix = matrix0 + matrix1 + matrix2 + matrix3;

			skinnedPosition[i] = transformPoint(skinMatrix, positions[i]);
			skinnedNormal[i] = transformVector(skinMatrix, normals[i]);
		}

		positionsAttribute->set(skinnedPosition);
		normalsAttribute->set(skinnedNormal);
	}

	void SkeletalMesh::CPUSkinUseMatrixPalette(const std::vector<Matrix4>& animationtPose)
	{
		uint32_t numVertices = static_cast<uint32_t>(positions.size());

		if (numVertices == 0)
		{
			return;
		}

		skinnedPosition.resize(numVertices);
		skinnedNormal.resize(numVertices);

		for (uint32_t i = 0; i < numVertices; i++)
		{
			Vector4i& jointIds = influenceJoints[i];
			Vector4& weight = weights[i];

			Vector3 position0 = transformPoint(animationtPose[jointIds.x] * weight.x, positions[i]);
			Vector3 position1 = transformPoint(animationtPose[jointIds.y] * weight.y, positions[i]);
			Vector3 position2 = transformPoint(animationtPose[jointIds.z] * weight.z, positions[i]);
			Vector3 position3 = transformPoint(animationtPose[jointIds.w] * weight.w, positions[i]);

			skinnedPosition[i] = position0 + position1 + position2 + position3;

			Vector3 normal0 = transformVector(animationtPose[jointIds.x] * weight.x, normals[i]);
			Vector3 normal1 = transformVector(animationtPose[jointIds.y] * weight.y, normals[i]);
			Vector3 normal2 = transformVector(animationtPose[jointIds.z] * weight.z, normals[i]);
			Vector3 normal3 = transformVector(animationtPose[jointIds.w] * weight.w, normals[i]);

			skinnedNormal[i] = normal0 + normal1 + normal2 + normal3;
		}

		positionsAttribute->set(skinnedPosition);
		normalsAttribute->set(skinnedNormal);
	}

	void SkeletalMesh::updateOpenGLBuffers()
	{
		if (positions.size() > 0)
		{
			positionsAttribute->set(positions);
		}

		if (normals.size() > 0)
		{
			normalsAttribute->set(normals);
		}

		if (texcoords.size() > 0)
		{
			texcoordsAttribute->set(texcoords);
		}

		if (weights.size() > 0)
		{
			weightsAttribute->set(weights);
		}

		if (influenceJoints.size() > 0)
		{
			influencesAttribute->set(influenceJoints);
		}

		if (indices.size() > 0)
		{
			indexBuffer->set(indices);
		}
	}

	void SkeletalMesh::bind(int32_t position, int32_t normal, int32_t texcoord, int32_t weights, int32_t influences)
	{
		if (position >= 0)
		{
			positionsAttribute->bindTo(position);
		}

		if (normal >= 0)
		{
			normalsAttribute->bindTo(normal);
		}

		if (texcoord >= 0)
		{
			texcoordsAttribute->bindTo(texcoord);
		}

		if (weights >= 0)
		{
			weightsAttribute->bindTo(weights);
		}

		if (influences >= 0)
		{
			influencesAttribute->bindTo(influences);
		}
	}

	void SkeletalMesh::unbind(int32_t position, int32_t normal, int32_t texcoord, int32_t weights, int32_t influences)
	{
		if (position >= 0)
		{
			positionsAttribute->unbindFrom(position);
		}

		if (normal >= 0)
		{
			normalsAttribute->unbindFrom(normal);
		}

		if (texcoord >= 0)
		{
			texcoordsAttribute->unbindFrom(texcoord);
		}

		if (weights >= 0)
		{
			weightsAttribute->unbindFrom(weights);
		}

		if (influences >= 0)
		{
			influencesAttribute->unbindFrom(influences);
		}
	}

	void SkeletalMesh::draw()
	{
		if (indices.size() > 0)
		{
			Renderer::draw(*indexBuffer, RenderMode::Triangles);
		}
		else
		{
			Renderer::draw(static_cast<uint32_t>(positions.size()), RenderMode::Triangles);
		}
	}

	void SkeletalMesh::drawInstanced(uint32_t numInstances)
	{
		if (indices.size() > 0)
		{
			Renderer::drawInstanced(*indexBuffer, RenderMode::Triangles, numInstances);
		}
		else
		{
			Renderer::drawInstanced(static_cast<uint32_t>(positions.size()), RenderMode::Triangles, numInstances);
		}
	}
}