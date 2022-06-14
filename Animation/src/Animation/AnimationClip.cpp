#include "AnimationClip.h"

#include <cstdint>

#include <Math/Math.h>
#include <Math/Transform.h>

using namespace Math;

namespace Animation
{
	template TAnimationClip<AnimationTransformTrack>;
	template TAnimationClip<FastAnimationTransformTrack>;

	template <typename TAnimationTransformTrack>
	TAnimationClip<TAnimationTransformTrack>::TAnimationClip()
	{
		name = "";
		startTime = 0.0f;
		endTime = 0.0f;
		bLooping = true;
	}

	template <typename TAnimationTransformTrack>
	uint32_t TAnimationClip<TAnimationTransformTrack>::getJointIdAtIndex(uint32_t index) const
	{
		return transformTracks[index].getJointId();
	}

	template <typename TAnimationTransformTrack>
	void TAnimationClip<TAnimationTransformTrack>::setJointIdAtIndex(uint32_t index, uint32_t jointId)
	{
		return transformTracks[index].setJointId(jointId);
	}

	template <typename TAnimationTransformTrack>
	uint32_t TAnimationClip<TAnimationTransformTrack>::getSize() const
	{
		return static_cast<uint32_t>(transformTracks.size());
	}

	template <typename TAnimationTransformTrack>
	float TAnimationClip<TAnimationTransformTrack>::sample(AnimationPose& outAnimationPose, float inTime)
	{
		if (getDuration() == 0.0f)
		{
			return 0.0f;
		}

		float time = adjustTimeToFitRange(inTime);

		uint32_t trackSize = static_cast<uint32_t>(transformTracks.size());

		for (uint32_t i = 0; i < trackSize; i++)
		{
			uint32_t jointId = transformTracks[i].getJointId();
			Transform localTransform = outAnimationPose.getLocalTransform(jointId);
			Transform animatedTransform = transformTracks[i].sample(localTransform, time, bLooping);
			outAnimationPose.setLocalTransform(jointId, animatedTransform);
		}

		return time;
	}

	template <typename TAnimationTransformTrack>
	TAnimationTransformTrack& TAnimationClip<TAnimationTransformTrack>::operator[](uint32_t jointId)
	{
		uint32_t trackSize = static_cast<uint32_t>(transformTracks.size());
		
		for (uint32_t i = 0; i < trackSize; i++)
		{
			if (transformTracks[i].getJointId() == jointId)
			{
				return transformTracks[i];
			}
		}

		transformTracks.emplace_back(TAnimationTransformTrack());
		TAnimationTransformTrack& transformTrack = transformTracks[transformTracks.size() - 1];
		transformTrack.setJointId(jointId);
		
		return transformTrack;
	}

	template <typename TAnimationTransformTrack>
	void TAnimationClip<TAnimationTransformTrack>::recalculateDuration()
	{
		startTime = 0.0f;
		endTime = 0.0f;

		bool bSetStartTime = false;
		bool bSetEndTime = false;

		uint32_t tracksSize = static_cast<uint32_t>(transformTracks.size());

		for (uint32_t i = 0; i < tracksSize; i++)
		{
			float trackStartTime = transformTracks[i].getStartTime();
			float trackEndTime = transformTracks[i].getEndTime();

			if (trackStartTime < startTime || !bSetStartTime)
			{
				startTime = trackStartTime;
				bSetStartTime = true;
			}

			if (trackEndTime > endTime || !bSetEndTime)
			{
				endTime = trackEndTime;
				bSetEndTime = true;
			}
		}
	}

	template <typename TAnimationTransformTrack>
	std::string TAnimationClip<TAnimationTransformTrack>::getName() const
	{
		return name;
	}

	template <typename TAnimationTransformTrack>
	void TAnimationClip<TAnimationTransformTrack>::setName(const std::string& newName)
	{
		name = newName;
	}

	template <typename TAnimationTransformTrack>
	float TAnimationClip<TAnimationTransformTrack>::getDuration() const
	{
		return endTime - startTime;
	}

	template <typename TAnimationTransformTrack>
	float TAnimationClip<TAnimationTransformTrack>::getStartTime() const
	{
		return startTime;
	}

	template <typename TAnimationTransformTrack>
	float TAnimationClip<TAnimationTransformTrack>::getEndTime() const
	{
		return endTime;
	}

	template <typename TAnimationTransformTrack>
	bool TAnimationClip<TAnimationTransformTrack>::isLooping() const
	{
		return bLooping;
	}

	template <typename TAnimationTransformTrack>
	void TAnimationClip<TAnimationTransformTrack>::setLooping(bool bInLooping)
	{
		bLooping = bInLooping;
	}

	template <typename TAnimationTransformTrack>
	float TAnimationClip<TAnimationTransformTrack>::adjustTimeToFitRange(float inTime)
	{
		float time = inTime;
		
		if (bLooping)
		{
			float duration = endTime - startTime;
			
			if (duration < 0.0f)
			{
				duration = 0.0f;
			}

			time = FMod(inTime - startTime, endTime - startTime);
			
			if (time < 0.0f)
			{
				time += duration;
			}

			time += startTime;
		}
		else
		{
			if (time < startTime)
			{
				time = startTime;
			}

			if (time > endTime)
			{
				time = endTime;
			}
		}

		return time;
	}

	FastAnimationClip optimizeAnimationClip(AnimationClip& input)
	{
		FastAnimationClip result;
		result.setName(input.getName());
		result.setLooping(input.isLooping());

		uint32_t size = input.getSize();
		for (uint32_t i = 0; i < size; i++)
		{
			uint32_t jointId = input.getJointIdAtIndex(i);
			result[jointId] = optimizeAnimationTransformTrack(input[jointId]);
		}

		result.recalculateDuration();
		return result;
	}
}