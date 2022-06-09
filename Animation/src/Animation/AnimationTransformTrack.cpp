#include "AnimationTransformTrack.h"

namespace Animation
{
	AnimationTransformTrack::AnimationTransformTrack()
	{
		jointId = 0;
	}

	uint32_t AnimationTransformTrack::getJointId() const
	{
		return jointId;
	}

	void AnimationTransformTrack::setJointId(uint32_t inJointId)
	{
		jointId = inJointId;
	}

	VectorTrack& AnimationTransformTrack::getPositionTrack()
	{
		return position;
	}

	QuaternionTrack& AnimationTransformTrack::getRotationTrack()
	{
		return rotation;
	}

	VectorTrack& AnimationTransformTrack::getScaleTrack()
	{
		return scale;
	}

	float AnimationTransformTrack::getStartTime() const
	{
		float startTime = 0.0f;
		bool bIsSet = false;

		if (position.frameCount() > 1)
		{
			startTime = position.getStartTime();
			bIsSet = true;
		}
		
		if (rotation.frameCount() > 1)
		{
			float rotationStartTime = rotation.getStartTime();
			
			if (rotationStartTime < startTime || bIsSet)
			{
				startTime = rotationStartTime;
				bIsSet = true;
			}
		}

		if (scale.frameCount() > 1)
		{
			float scaleStartTime = scale.getStartTime();
			
			if (scaleStartTime < startTime || !bIsSet)
			{
				startTime = scaleStartTime;
				bIsSet = true;
			}
		}

		return startTime;
	}

	float AnimationTransformTrack::getEndTime() const
	{
		float endTime = 0.0f;
		bool bIsSet = false;

		if (position.frameCount() > 1)
		{
			endTime = position.getEndTime();
			bIsSet = true;
		}

		if (rotation.frameCount() > 1)
		{
			float rotationEndTime = rotation.getEndTime();

			if (rotationEndTime > endTime || bIsSet)
			{
				endTime = rotationEndTime;
				bIsSet = true;
			}
		}

		if (scale.frameCount() > 1)
		{
			float scaleEndTime = scale.getEndTime();

			if (scaleEndTime > endTime || !bIsSet)
			{
				endTime = scaleEndTime;
				bIsSet = true;
			}
		}

		return endTime;
	}

	bool AnimationTransformTrack::isValid() const
	{
		return position.frameCount() > 1 ||
			   rotation.frameCount() > 1 ||
			   scale.frameCount() > 1;
	}

	Transform AnimationTransformTrack::sample(const Transform& reference, float time, bool bLooping)
	{
		// Assign default values
		Transform result = reference;

		// Only if valid
		if (position.frameCount() > 1)
		{
			result.position = position.sample(time, bLooping);
		}

		// Only if valid
		if (rotation.frameCount() > 1)
		{
			result.rotation = rotation.sample(time, bLooping);
		}

		// Only if valid
		if (scale.frameCount() > 1)
		{
			result.scale = scale.sample(time, bLooping);
		}

		return result;
	}
}

