#include "AnimationClip.h"

#include <cstdint>

#include <Math/Math.h>
#include <Math/Transform.h>

using namespace Math;

namespace Animation
{
	
	AnimationClip::AnimationClip()
	{
		name = "";
		startTime = 0.0f;
		endTime = 0.0f;
		bLooping = true;
	}

	uint32_t AnimationClip::getJointIdAtIndex(uint32_t index)
	{
		return transformTracks[index].getJointId();
	}

	void AnimationClip::setJointIdAtIndex(uint32_t index, uint32_t jointId)
	{
		return transformTracks[index].setJointId(jointId);
	}

	uint32_t AnimationClip::getSize()
	{
		return static_cast<uint32_t>(transformTracks.size());
	}

	float AnimationClip::sample(AnimationPose& outAnimationPose, float inTime)
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

	AnimationTransformTrack& AnimationClip::operator[](uint32_t jointId)
	{
		uint32_t trackSize = static_cast<uint32_t>(transformTracks.size());
		
		for (uint32_t i = 0; i < trackSize; i++)
		{
			if (transformTracks[i].getJointId() == jointId)
			{
				return transformTracks[i];
			}
		}

		transformTracks.emplace_back(AnimationTransformTrack());
		AnimationTransformTrack& transformTrack = transformTracks[transformTracks.size() - 1];
		transformTrack.setJointId(jointId);
		
		return transformTrack;
	}

	void AnimationClip::recalculateDuration()
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

	std::string AnimationClip::getName() const
	{
		return name;
	}

	void AnimationClip::setName(const std::string& newName)
	{
		name = newName;
	}

	float AnimationClip::getDuration() const
	{
		return endTime - startTime;
	}

	float AnimationClip::getStartTime() const
	{
		return startTime;
	}

	float AnimationClip::getEndTime() const
	{
		return endTime;
	}

	bool AnimationClip::isLooping() const
	{
		return bLooping;
	}

	void AnimationClip::setLooping(bool bInLooping)
	{
		bLooping = bInLooping;
	}

	float AnimationClip::adjustTimeToFitRange(float inTime)
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
}