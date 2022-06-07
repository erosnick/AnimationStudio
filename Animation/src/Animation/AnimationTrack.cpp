#include "AnimationTrack.h"
#include "AnimationTrackHelpers.h"

namespace Animation
{
	template AnimationTrack<float, 1>;
	template AnimationTrack<Math::Vector3, 3>;
	template AnimationTrack<Math::Quaternion, 4>;

	template <typename T, int32_t N>
	AnimationTrack<T, N>::AnimationTrack()
	{
		interpolation = interpolation::Linear;
	}

	template <typename T, int32_t N>
	void AnimationTrack<T, N>::resize(uint32_t size)
	{
		keyframes.resize(size);
	}

	template <typename T, int32_t N>
	uint32_t AnimationTrack<T, N>::frameCount() const
	{
		return keyframes.size();
	}

	template <typename T, int32_t N>
	Interpolation AnimationTrack<T, N>::getInterpolation() const
	{
		return interpolation;
	}

	template <typename T, int32_t N>
	void AnimationTrack<T, N>::setInterpolation(Interpolation inInterpolation)
	{
		interpolation = inInterpolation;
	}

	template <typename T, int32_t N>
	float AnimationTrack<T, N>::getEndTime() const
	{
		return keyframes[keyframes.size() - 1].time;
	}

	template <typename T, int32_t N>
	T AnimationTrack<T, N>::sample(float time, bool bLooping)
	{
		if (interpolation == Interpolation::Constant)
		{
			return sampleConstant(time, bLooping);
		}
		else if (interpolation == Interpolation::Linear)
		{
			return sampleLinear(time, bLooping);
		}
		else
		{
			return sampleCubic(time, bLooping);
		}
	}

	template <typename T, int32_t N>
	AnimationKeyFrame<N>& AnimationTrack<T, N>::operator[](uint32_t index)
	{
		return keyframes[index];
	}

	template <typename T, int32_t N>
	T AnimationTrack<T, N>::hermite(float time, const T& p1, const T& s1, const T& p2, const T& s2)
	{
		AnimationTrackHelpers::Neighborhood(p1, p2);
		
		T result = p1 * ((1.0f + 2.0f * t) * ((1.0f - t) * (1.0f - t))) +
				   s1 * (t * ((1.0f - t) * (1.0f - t))) +
				   p2 * ((t * t) * (3.0f - 2.0f * t)) +
				   s2 * ((t * t) * (t - 1.0f));
		
		return AnimationTrackHelpers::adjustHermiteResult(result);
	}

	template <typename T, int32_t N>
	int32_t AnimationTrack<T, N>::frameIndex(float time, bool bLooping) const
	{
		uint32_t size = static_cast<uint32_t>(keyframes.size());

		if (size <= 1)
		{
			return -1;
		}

		if (bLooping)
		{
			float startTime = keyframes[0].time;
			float endTime = keyframes[size - 1].time;
			float duration = endTime - startTime;

			time = FMod(time - startTime, duration) + startTime;

			if (time < 0.0f)
			{
				time += endTime - startTime;
			}

			time = time + startTime;
		}
		else
		{
			if (time <= keyframes[0].time)
			{
				return 0;
			}

			if (time >= keyframes[size - 2].time)
			{
				return static_cast<int32_t>(size - 2);
			}
		}

		for (uint32_t i = 0; i < size; i++)
		{
			if (time >= keyframes[i].time)
			{
				return i;
			}
		}

		// Invalid code, we should not reach here!
		return -1;
	}

	template <typename T, int32_t N>
	float AnimationTrack<T, N>::adjustTimeToFitTrack(float time, bool bLooping) const
	{
		
	}
}