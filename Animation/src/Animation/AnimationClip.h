#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "AnimationPose.h"
#include "AnimationTransformTrack.h"

namespace Animation
{
	template <typename TAnimationTransformTrack>
	class TAnimationClip
	{
	public:
		TAnimationClip();
		
		uint32_t getJointIdAtIndex(uint32_t index) const;
		void setJointIdAtIndex(uint32_t index, uint32_t jointId);
		uint32_t getSize() const;

		float sample(AnimationPose& outAnimationPose, float inTime);
		TAnimationTransformTrack& operator[](uint32_t jointId);

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
		std::vector<TAnimationTransformTrack> transformTracks;
		std::string name;
		float startTime;
		float endTime;
		bool bLooping;
	};

	using AnimationClip = TAnimationClip<AnimationTransformTrack>;
	using FastAnimationClip = TAnimationClip<FastAnimationTransformTrack>;

	FastAnimationClip optimizeAnimationClip(AnimationClip& input);
}