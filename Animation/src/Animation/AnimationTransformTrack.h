#pragma once

#include <cstdint>
#include "AnimationTrack.h"
#include "FastAnimationTrack.h"
#include <Math/Transform.h>

namespace Animation
{
	template <typename TVectorTrack, typename TQuaternionTrack>
	class TAnimationTransformTrack
	{
	public:
		TAnimationTransformTrack();
		uint32_t getJointId() const;
		void setJointId(uint32_t inJointId);
		TVectorTrack& getPositionTrack();
		TQuaternionTrack& getRotationTrack();
		TVectorTrack& getScaleTrack();
		const TVectorTrack& getPositionTrack() const;
		const TQuaternionTrack& getRotationTrack() const;
		const TVectorTrack& getScaleTrack() const;
		float getStartTime() const;
		float getEndTime() const;
		bool isValid() const;
		
		Transform sample(const Transform& reference, float time, bool bLooping) const;
	protected:
		uint32_t jointId;
		TVectorTrack position;
		TQuaternionTrack rotation;
		TVectorTrack scale;
	};

	using AnimationTransformTrack = TAnimationTransformTrack<VectorTrack, QuaternionTrack>;
	using FastAnimationTransformTrack = TAnimationTransformTrack<FastVectorTrack, FastQuaternionTrack>;

	FastAnimationTransformTrack optimizeAnimationTransformTrack(AnimationTransformTrack& input);
}