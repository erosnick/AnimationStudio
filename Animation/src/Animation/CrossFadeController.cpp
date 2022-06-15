#include "CrossFadeController.h"
#include "Blending.h"

namespace Animation
{
	template CrossFadeController<AnimationClip>;
	template CrossFadeController<FastAnimationClip>;

	template <typename TAnimationClip>
	CrossFadeController<TAnimationClip>::CrossFadeController()
	{
		animationClip = nullptr;
		time = 0.0f;
		bWasSkeletonSet = false;
	}
	
	template <typename TAnimationClip>
	CrossFadeController<TAnimationClip>::CrossFadeController(const Skeleton& inSkeleton)
	{
		animationClip = nullptr;
		time = 0.0f;
		setSkeleton(skeleton);
	}

	template <typename TAnimationClip>
	void CrossFadeController<TAnimationClip>::setSkeleton(const Skeleton& inSkeleton)
	{
		skeleton = inSkeleton;
		animationPose = skeleton.getRestPose();
		bWasSkeletonSet = true;
	}

	template <typename TAnimationClip>
	void CrossFadeController<TAnimationClip>::play(TAnimationClip* target)
	{
		targets.clear();
		animationClip = target;
		animationPose = skeleton.getRestPose();
		time = target->getStartTime();
	}

	template <typename TAnimationClip>
	void CrossFadeController<TAnimationClip>::fadeTo(TAnimationClip* target, float fadeTime)
	{
		// A fade target is only valid if it is not the first or last item in the fade
		// list.Assuming these conditions are met, the FadeTo function adds the provided
		// animation clip and duration to the fade list
		if (animationClip == nullptr)
		{
			play(target);
			return;
		}

		if (targets.size() >= 1)
		{
			auto lastAnimationClip = targets[targets.size() - 1].animationClip;
			if (lastAnimationClip == target)
			{
				return;
			}
		}
		else
		{
			if (animationClip == target)
			{
				return;
			}
		}

		targets.emplace_back(CrossFadeTarget<TAnimationClip>(target, skeleton.getRestPose(), fadeTime));
	}

	template <typename TAnimationClip>
	void CrossFadeController<TAnimationClip>::update(float deltaTime)
	{
		if (animationClip == nullptr || !bWasSkeletonSet)
		{
			return;
		}

		// Set the current animation as the target animation and remove the fade object if an
		// animation has finished fading.Only one target is removed per frame.If you want to
		// remove all the faded-out targets, change the loop to go backward
		uint32_t numTargets = static_cast<uint32_t>(targets.size());

		for (uint32_t i = 0; i < numTargets; i++)
		{
			float duration = targets[i].duration;

			if (targets[i].elapsed >= duration)
			{
				animationClip = targets[i].animationClip;
				time = targets[i].time;
				animationPose = targets[i].animationPose;
				targets.erase(targets.begin() + i);
				break;
			}
		}

		numTargets = static_cast<uint32_t>(targets.size());
		animationPose = skeleton.getRestPose();
		time = animationClip->sample(animationPose, time + deltaTime);

		for (uint32_t i = 0; i < numTargets; i++)
		{
			CrossFadeTarget<TAnimationClip>& target = targets[i];
			target.time = target.animationClip->sample(target.animationPose, target.time + deltaTime);

			target.elapsed += deltaTime;
			
			float t = target.elapsed / target.duration;
			
			if (t > 1.0f)
			{
				t = 1.0f;
			}

			blend(animationPose, animationPose, target.animationPose, t, -1);
		}
	}

	template <typename TAnimationClip>
	AnimationPose& CrossFadeController<TAnimationClip>::getCurrentAnimationPose()
	{
		return animationPose;
	}

	template <typename TAnimationClip>
	const AnimationPose& CrossFadeController<TAnimationClip>::getCurrentAnimationPose() const
	{
		return animationPose;
	}

	template <typename TAnimationClip>
	TAnimationClip* CrossFadeController<TAnimationClip>::getCurrentAnimationClip()
	{
		return animationClip;
	}
}