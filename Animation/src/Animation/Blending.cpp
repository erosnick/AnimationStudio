#include "Blending.h"

namespace Animation
{
	AnimationPose makeAdditivePose(const Skeleton& skeleton, AnimationClip& animationClip)
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