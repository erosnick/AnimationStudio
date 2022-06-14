#pragma once

#include "Skeleton.h"
#include "AnimationPose.h"
#include "AnimationClip.h"
#include "CrossFadeTarget.h"

#include <vector>

namespace Animation
{
	class CrossFadeController
	{
	public:
		CrossFadeController();
		CrossFadeController(const Skeleton& inSkeleton);
		void setSkeleton(const Skeleton& inSkeleton);
		void play(const std::shared_ptr<AnimationClip>& target);
		void fadeTo(const std::shared_ptr<AnimationClip>& target, float fadeTime);
		void update(float deltaTime);
		AnimationPose& getCurrentAnimationPose();
		const AnimationPose& getCurrentAnimationPose() const;
		std::shared_ptr<AnimationClip>& getCurrentAnimationClip();
		const std::shared_ptr<AnimationClip>& getCurrentAnimationClip() const;
	protected:
		std::vector<CrossFadeTarget> targets;
		std::shared_ptr<AnimationClip> animationClip;
		float time;
		AnimationPose animationPose;
		Skeleton skeleton;
		bool bWasSkeletonSet;
	};
}