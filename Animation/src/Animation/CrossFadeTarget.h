#pragma once

#include "AnimationPose.h"
#include "AnimationClip.h"

#include <memory>

namespace Animation
{
	template <typename TAnimationClip>
	struct CrossFadeTarget
	{
		inline CrossFadeTarget() :
			animationClip(nullptr),
			time(0.0f),
			duration(0.0f), 
			elapsed(0.0f)
		{}
		
		inline CrossFadeTarget(TAnimationClip* target, const AnimationPose& inAnimationPose, float inDuration)
		: animationClip(target),
		  time(target->getStartTime()),
		  animationPose(inAnimationPose),
		  duration(inDuration),
		  elapsed(0.0f)
		{}
		
		AnimationPose animationPose;
		TAnimationClip* animationClip;
		float time;
		float duration;
		float elapsed;
	};
}