#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "AnimationPose.h"
#include "AnimationTransformTrack.h"

namespace Animation
{
	class AnimationClip
	{
	public:
		AnimationClip();
		
		uint32_t getJointIdAtIndex(uint32_t index);
		void setJointIdAtIndex(uint32_t index, uint32_t jointId);
		uint32_t getSize();

		float sample(AnimationPose& outAnimationPose, float inTime);
		AnimationTransformTrack& operator[](uint32_t jointId);

		void recalculateDuration();
		std::string getName() const;
		void setName(const std::string& newName);
		float getDuration() const;
		float getStartTime() const;
		float getEndTime() const;
		bool isLooping() const;
		void setLooping(bool bInLooping);
		
	protected:
		float adjustTimeToFitRange(float inTime);
		
	protected:
		std::vector<AnimationTransformTrack> transformTracks;
		std::string name;
		float startTime;
		float endTime;
		bool bLooping;
	};
}