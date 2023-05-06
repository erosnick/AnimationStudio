#include "AnimationTransformTrack.h"

namespace Animation
{
	template TAnimationTransformTrack<VectorTrack, QuaternionTrack>;
	template TAnimationTransformTrack<FastVectorTrack, FastQuaternionTrack>;
	
	template <typename TVectorTrack, typename TQuaternionTrack>
	TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::TAnimationTransformTrack()
	{
		jointId = 0;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	uint32_t TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getJointId() const
	{
		return jointId;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	void TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::setJointId(uint32_t inJointId)
	{
		jointId = inJointId;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	TVectorTrack& TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getPositionTrack()
	{
		return position;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	TQuaternionTrack& TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getRotationTrack()
	{
		return rotation;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	TVectorTrack& TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getScaleTrack()
	{
		return scale;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	const TQuaternionTrack& TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getRotationTrack() const
	{
		return rotation;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	const TVectorTrack& TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getPositionTrack() const
	{
		return position;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	const TVectorTrack& TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getScaleTrack() const
	{
		return scale;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	float TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getStartTime() const
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

	template <typename TVectorTrack, typename TQuaternionTrack>
	float TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::getEndTime() const
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

	template <typename TVectorTrack, typename TQuaternionTrack>
	bool TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::isValid() const
	{
		return position.frameCount() > 1 ||
			   rotation.frameCount() > 1 ||
			   scale.frameCount() > 1;
	}

	template <typename TVectorTrack, typename TQuaternionTrack>
	Transform TAnimationTransformTrack<TVectorTrack, TQuaternionTrack>::sample(const Transform& reference, float time, bool bLooping) const
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

	FastAnimationTransformTrack optimizeAnimationTransformTrack(AnimationTransformTrack& input)
	{
		FastAnimationTransformTrack result;
		
		result.setJointId(input.getJointId());
		result.getPositionTrack() = optimizeAnimationTrack<Vector3, 3>(input.getPositionTrack());
		result.getRotationTrack() = optimizeAnimationTrack<Quaternion, 4>(input.getRotationTrack());
		result.getScaleTrack() = optimizeAnimationTrack<Vector3, 3>(input.getScaleTrack());

		return result;
	}
}

