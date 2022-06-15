#include "Blending.h"

namespace Animation
{
	bool isInHierarchy(const AnimationPose& animationPose, uint32_t parent, uint32_t search)
	{
		// To check whether one joint is the descendant of another, follow the descendant joint
		// all the way up the hierarchy until the root node.If any of the nodes encountered in
		// this hierarchy are the node that you are checking against, return true
		if (search == parent)
		{
			return true;
		}

		int32_t currentParent = animationPose.getParent(search);

		while (currentParent >= 0)
		{
			if (currentParent == parent)
			{
				return true;
			}

			currentParent = animationPose.getParent(currentParent);
		}

		return false;
	}

	void blend(AnimationPose& result, const AnimationPose& a, const AnimationPose& b, float t, int32_t blendRoot)
	{
		// If two animations are blended using the whole hierarchy, the root argument to Blend
		// will be negative.With a negative joint for the blend root, the Blend function skips the
		// isInHierarchy check
		uint32_t numJoints = result.getSize();

		for (uint32_t i = 0; i < numJoints; i++)
		{
			if (blendRoot >= 0)
			{
				if (!isInHierarchy(result, blendRoot, i))
				{
					continue;
				}
			}

			result.setLocalTransform(i, lerp(a.getLocalTransform(i), b.getLocalTransform(i), t));
		}
	}
	
	template AnimationPose makeAdditivePose<AnimationClip>(const Skeleton& skeleton, AnimationClip& animationClip);
	template AnimationPose makeAdditivePose<FastAnimationClip>(const Skeleton& skeleton, FastAnimationClip& animationClip);
	
	template <typename TAniamtionClip>
	AnimationPose makeAdditivePose(const Skeleton& skeleton, TAniamtionClip& animationClip)
	{
		AnimationPose result = skeleton.getRestPose();
		animationClip.sample(result, animationClip.getStartTime());
		return result;
	}

	void add(AnimationPose& outPose, const AnimationPose& inPose, const AnimationPose& additivePose, const AnimationPose& additiveBasePose, int32_t blendRoot)
	{
		uint32_t numJoints = additivePose.getSize();
		
		for (uint32_t i = 0; i < numJoints; i++)
		{
			Transform input = inPose.getLocalTransform(i);
			Transform additive = additivePose.getLocalTransform(i);
			Transform additiveBase = additiveBasePose.getLocalTransform(i);

			if (blendRoot >= 0 && !isInHierarchy(additivePose, blendRoot, i))
			{
				continue;
			}

			// outPose = inPose + (additivePose - additiveBasePose)
			Quaternion inputRotation = input.rotation;
			Quaternion addtiveRotation = additive.rotation;
			Quaternion inverseAdditiveBaseRotation = inverse(additiveBase.rotation);
			
			Transform result(input.position + (additive.position - additiveBase.position), 
							 normalized(inputRotation * inverseAdditiveBaseRotation * addtiveRotation),
							 input.scale + (additive.scale - additiveBase.scale));

			outPose.setLocalTransform(i, result);
		}
	}
}