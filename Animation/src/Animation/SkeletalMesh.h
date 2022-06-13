#pragma once

#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

#include <Renderer/Attribute.h>
#include <Renderer/IndexBuffer.h>

#include "Skeleton.h"
#include "AnimationPose.h"

#include <cstdint>
#include <vector>
#include <memory>

using namespace Math;
using namespace Renderer;

namespace Animation
{
	class SkeletalMesh
	{
	public:
		SkeletalMesh();
		SkeletalMesh(const SkeletalMesh& mesh);
		SkeletalMesh& operator=(const SkeletalMesh& mesh);
		~SkeletalMesh();

		std::vector<Vector3>& getPositions();
		std::vector<Vector3>& getNormals();
		std::vector<Vector3>& getTangents();
		std::vector<Vector2>& getTexcoords();
		std::vector<Vector4>& getWeights();
		std::vector<Vector4i>& getInfluenceJoints();
		std::vector<uint32_t>& getIndices();

		const std::vector<Vector3>& getPositions() const;
		const std::vector<Vector3>& getNormals() const;
		const std::vector<Vector3>& getTangents() const;
		const std::vector<Vector2>& getTexcoords() const;
		const std::vector<Vector4>& getWeights() const;
		const std::vector<Vector4i>& getInfluenceJoints() const;
		const std::vector<uint32_t>& getIndices() const;
		
		void CPUSkinUseTransform(const Skeleton& skeleton, const AnimationPose& animationPose);
		void CPUSkinUseMatrixPalette(const Skeleton& skeleton, const AnimationPose& animationPose);
		void CPUSkinUseMatrixPalette(const std::vector<Matrix4>& animationtPose);
		
		void updateOpenGLBuffers();
		
		void bind(int32_t position, int32_t normal, int32_t texcoord, int32_t weights, int32_t influences);
		void unbind(int32_t position, int32_t normal, int32_t texcoord, int32_t weights, int32_t influences);
		
		void draw();
		void drawInstanced(uint32_t numInstances);

	protected:
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector3> tangents;
		std::vector<Vector2> texcoords;
		std::vector<Vector4> weights;
		std::vector<Vector4i> influenceJoints;
		std::vector<uint32_t> indices;

		std::shared_ptr<Attribute<Vector3>> positionsAttribute;
		std::shared_ptr<Attribute<Vector3>> normalsAttribute;
		std::shared_ptr<Attribute<Vector2>> texcoordsAttribute;
		std::shared_ptr<Attribute<Vector4>> weightsAttribute;
		std::shared_ptr<Attribute<Vector4i>> influencesAttribute;
		std::shared_ptr<IndexBuffer> indexBuffer;

		// Additional copy of pose and normal data, 
		// as well as matrix palette for CPU skinning
		std::vector<Vector3> skinnedPosition;
		std::vector<Vector3> skinnedNormal;
		std::vector<Matrix4> animationPosePalette;
	};
}