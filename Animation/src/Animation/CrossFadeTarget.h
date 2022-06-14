#pragma once

#include "AnimationPose.h"
#include "AnimationClip.h"

#include <memory>

namespace Animation
{
	struct CrossFadeTarget
	{
		inline CrossFadeTarget() :
			animationClip(nullptr),
			time(0.0f),
			duration(0.0f), 
			elapsed(0.0f)
		{}
		
		inline CrossFadeTarget(const std::shared_ptr<AnimationClip>& target, const AnimationPose& inAnimationPose, float inDuration)
		: animationClip(target),
		  time(target->getStartTime()),
		  animationPose(inAnimationPose),
		  duration(inDuration),
		  elapsed(0.0f)
		{}
		
		AnimationPose animationPose;
		std::shared_ptr<AnimationClip> animationClip;
		float time;
		float duration;
		float elapsed;
	};
}