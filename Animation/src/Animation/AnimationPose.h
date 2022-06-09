#pragma once

#include <cstdint>

namespace Animation
{
	class AnimationPose
	{
	public:
		AnimationPose();
		AnimationPose(const AnimationPose& other);
		AnimationPose(int32_t numBones);
		~AnimationPose();

		AnimationPose& operator=(const AnimationPose& other);

		void resize(uint32_t newSize);
		uint32_t getSize() const;

		int32_t getParent(uint32_t index);
		void setParent(uint32_t index, int parent);
	};
}