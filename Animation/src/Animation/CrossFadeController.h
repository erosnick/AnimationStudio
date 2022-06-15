#pragma once

#include "Skeleton.h"
#include "AnimationPose.h"
#include "AnimationClip.h"
#include "CrossFadeTarget.h"

#include <vector>

namespace Animation
{
	template <typename TAnimationClip>
	class CrossFadeController
	{
	public:
		CrossFadeController();
		CrossFadeController(const Skeleton& inSkeleton);
		void setSkeleton(const Skeleton& inSkeleton);
		void play(TAnimationClip* target);
		void fadeTo(TAnimationClip* target, float fadeTime);
		void update(float deltaTime);
		AnimationPose& getCurrentAnimationPose();
		const AnimationPose& getCurrentAnimationPose() const;
		TAnimationClip* getCurrentAnimationClip();
	protected:
		std::vector<CrossFadeTarget<TAnimationClip>> targets;
		TAnimationClip* animationClip;
		float time;
		AnimationPose animationPose;
		Skeleton skeleton;
		bool bWasSkeletonSet;
	};
}