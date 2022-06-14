#include "FastAnimationTrack.h"
#include <Math/Math.h>

namespace Animation
{
	template FastAnimationTrack<float, 1>;
	template FastAnimationTrack<Vector3, 3>;
	template FastAnimationTrack<Quaternion, 4>;

	template <typename T, int32_t N>
	void FastAnimationTrack<T, N>::updateIndexLookupTable()
	{
		int32_t numFrames = static_cast<int32_t>(AnimationTrack<T, N>::keyframes.size());

		if (numFrames <= 1)
		{
			return;
		}

		// Since the class has 60 samples for every second of animation, multiply the duration by 60 :
		float duration = AnimationTrack<T, N>::getEndTime() - AnimationTrack<T, N>::getStartTime();
		uint32_t numSamples = static_cast<uint32_t>(duration * 60.0f);
		sampledFrames.resize(numSamples);

		// For each sample, find the time of the sample along the track. To find the time,
		// multiply the normalized iterator by the animation duration and add the start
		// time of the animation to it:
		for (uint32_t i = 0; i < numSamples; i++)
		{
			float normalized = static_cast<float>(i) / static_cast<float>(numSamples - 1);
			float frameTime = normalized * duration + AnimationTrack<T, N>::getStartTime();

			// Finally, it's time to find the frame index for each given time.
			// Find the frame that comes before the sampling time for this 
			// iteration and record it in the sampledFrames vector. If the
			// sampled frame is the last frame, return the index right before
			// the last index. Remember, the frameIndex function should never
			// return the last frame:
			int32_t frameIndex = 0;
			for (int32_t j = numFrames - 1; j >= 0; j--)
			{
				if (frameTime >= AnimationTrack<T, N>::keyframes[j].time)
				{
					frameIndex = j;

					if (frameIndex >= numFrames - 2)
					{
						frameIndex = numFrames - 2;
					}
					break;
				}
			}
			sampledFrames[i] = frameIndex;
		}
	}

	template <typename T, int32_t N>
	int32_t FastAnimationTrack<T, N>::frameIndex(float time, bool bLooping)
	{
		// The FrameIndex function is responsible for finding the frame right before a given time.
		// The optimized FastTrack class uses a lookup array instead of looping through every
		// frame of the track.All input times have a very similar performance cost.
		std::vector<AnimationKeyFrame<N>>& keyframes = AnimationTrack<T, N>::keyframes;

		uint32_t size = static_cast<uint32_t>(keyframes.size());

		if (size <= 1)
		{
			return -1;
		}

		float adjustedTime = time;

		if (bLooping)
		{
			float startTime = AnimationTrack<T, N>::keyframes[0].time;
			float endTime = AnimationTrack<T, N>::keyframes[size - 1].time;
			float duration = endTime - startTime;
			
			adjustedTime = Math::FMod(adjustedTime - startTime, duration);

			if (adjustedTime < 0.0f)
			{
				adjustedTime += duration;
			}
			adjustedTime += startTime;
		}
		else
		{
			if (time <= keyframes[0].time)
			{
				return 0;
			}

			if (time >= keyframes[size - 2].time)
			{
				return static_cast<int32_t>(size) - 2;
			}
		}
		
		float duration = AnimationTrack<T, N>::getEndTime() - AnimationTrack<T, N>::getStartTime();

		float normalized = adjustedTime / duration;
		uint32_t numSamples = static_cast<uint32_t>(duration * 60.0f);
		uint32_t index = static_cast<uint32_t>(normalized * numSamples);

		if (index >= sampledFrames.size())
		{
			return -1;
		}

		return sampledFrames[index];
	}
	
	template FastAnimationTrack<float, 1> optimizeAnimationTrack(AnimationTrack<float, 1>& input);
	template FastAnimationTrack<Vector3, 3> optimizeAnimationTrack(AnimationTrack<Vector3, 3>& input);
	template FastAnimationTrack<Quaternion, 4> optimizeAnimationTrack(AnimationTrack<Quaternion, 4>& input);

	template <typename T, int32_t N>
	FastAnimationTrack<T, N> optimizeAnimationTrack(AnimationTrack<T, N>& input)
	{
		FastAnimationTrack<T, N> result;
		result.setInterpolation(input.getInterpolation());

		uint32_t frameCount = input.frameCount();
		result.resize(frameCount);

		for (uint32_t i = 0; i < frameCount; i++)
		{
			result[i] = input[i];
		}

		result.updateIndexLookupTable();

		return result;
	}
}