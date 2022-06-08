#pragma once

#include <cstdint>
#include "AnimationTrack.h"
#include <Math/Transform.h>

namespace Animation
{
	class AnimationTransformTrack
	{
	public:
		AnimationTransformTrack();
		uint32_t getBoneId() const;
		void setBoneId(uint32_t inBoneId);
		VectorTrack& getPositionTrack();
		QuaternionTrack& getRotationTrack();
		VectorTrack& getScaleTrack();
		float getStartTime() const;
		float getEndTime() const;
		bool isValid() const;
		
		Transform sample(const Transform& reference, float time, bool bLooping);
	protected:
		uint32_t boneId;
		VectorTrack position;
		QuaternionTrack rotation;
		VectorTrack scale;
	};
}