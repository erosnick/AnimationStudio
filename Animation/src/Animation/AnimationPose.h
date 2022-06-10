#pragma once

#include <cstdint>
#include <vector>

#include <Math/Transform.h>

using namespace Math;

namespace Animation
{
	class AnimationPose
	{
	public:
		AnimationPose();
		AnimationPose(const AnimationPose& other);
		AnimationPose(int32_t numJoints);
		~AnimationPose();

		AnimationPose& operator=(const AnimationPose& other);

		void resize(uint32_t newSize);
		uint32_t getSize() const;

		int32_t getParent(uint32_t index) const;
		void setParent(uint32_t index, int parent);

		Transform getLocalTransform(uint32_t index);
		void setLocalTransform(uint32_t index, const Transform& transform);

		Transform getGlobalTransform(uint32_t index) const;
		Transform operator[](uint32_t index);

		void getMatrixPalette(std::vector<Matrix4>& out);

		bool operator==(const AnimationPose& other);
		bool operator!=(const AnimationPose& other);

	protected:
		std::vector<Transform> joints;
		std::vector<int32_t> parents;
	};
}