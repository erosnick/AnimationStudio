#include "AnimationTrack.h"
#include "AnimationTrackHelpers.h"
#include <Math/Math.h>

namespace Animation
{
	template AnimationTrack<float, 1>;
	template AnimationTrack<Math::Vector3, 3>;
	template AnimationTrack<Math::Quaternion, 4>;

	template <typename T, int32_t N>
	AnimationTrack<T, N>::AnimationTrack()
	{
		interpolation = Interpolation::Linear;
	}

	template <typename T, int32_t N>
	void AnimationTrack<T, N>::resize(uint32_t size)
	{
		keyframes.resize(size);
	}

	template <typename T, int32_t N>
	uint32_t AnimationTrack<T, N>::frameCount() const
	{
		return static_cast<uint32_t>(keyframes.size());
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
	float AnimationTrack<T, N>::getStartTime() const
	{
		return keyframes[0].time;
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
	T AnimationTrack<T, N>::sampleConstant(float time, bool bLooping)
	{
		int32_t frame = frameIndex(time, bLooping);

		if (frame < 0 || frame >= keyframes.size())
		{
			return T();
		}
		
		return cast(&keyframes[frame].value[0]);
	}

	template <typename T, int32_t N>
	T AnimationTrack<T, N>::sampleLinear(float time, bool bLooping)
	{
		int32_t currentFrame = frameIndex(time, bLooping);

		if (currentFrame < 0 || currentFrame >= (keyframes.size() - 1))
		{
			return T();
		}

		int32_t nextFrame = currentFrame + 1;

		float trackTime = adjustTimeToFitTrack(time, bLooping);
		float currentFrameTime = keyframes[currentFrame].time;
		float frameDelta = keyframes[nextFrame].time - currentFrameTime;

		if (frameDelta <= 0.0f)
		{
			return T();
		}

		float t = (trackTime - currentFrameTime) / frameDelta;

		T start = cast(&keyframes[currentFrame].value[0]);
		T end = cast(&keyframes[nextFrame].value[0]);
	
		return AnimationTrackHelpers::interpolate(start, end, t);
	}

	template <typename T, int32_t N>
	T AnimationTrack<T, N>::sampleCubic(float time, bool bLooping)
	{
		int32_t currentFrame = frameIndex(time, bLooping);

		if (currentFrame < 0 || currentFrame >= keyframes.size() - 1)
		{
			return T();
		}

		int32_t nextFrame = currentFrame + 1;

		float trackTime = adjustTimeToFitTrack(time, bLooping);
		float currentFrameTime = keyframes[currentFrame].time;
		float frameDelta = keyframes[nextFrame].time - currentFrameTime;

		if (frameDelta <= 0.0f)
		{
			return T();
		}

		float t = (trackTime - currentFrameTime) / frameDelta;
		
		size_t size = sizeof(float);

		T point1 = cast(&keyframes[currentFrame].value[0]);
		T slope1; // keyframes[currentFrame].out * frameDelta;
		memcpy_s(&slope1, sizeof(T), keyframes[currentFrame].out, N * size);
		slope1 = slope1 * frameDelta;

		T point2 = cast(&keyframes[nextFrame].value[0]);
		T slope2; // keyframes[nextFrame].out * frameDelta;
		memcpy_s(&slope2, sizeof(T), keyframes[nextFrame].out, N * size);
		slope2 = slope2 * frameDelta;

		return hermite(t, point1, slope1, point2, slope2);
	}

	template <typename T, int32_t N>
	T AnimationTrack<T, N>::hermite(float time, const T& p1, const T& s1, const T& p2, const T& s2)
	{
		T adjustedP2 = p2;
		
		AnimationTrackHelpers::neighborhood(p1, adjustedP2);
		
		T result = p1 * ((1.0f + 2.0f * time) * ((1.0f - time) * (1.0f - time))) +
				   s1 * (time * ((1.0f - time) * (1.0f - time))) +
				   adjustedP2 * ((time * time) * (3.0f - 2.0f * time)) +
				   s2 * ((time * time) * (time - 1.0f));
		
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

		for (uint32_t i = size - 1; i >= 0 ; i--)
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
		uint32_t size = static_cast<uint32_t>(keyframes.size());

		if (size <= 1)
		{
			return 0.0f;
		}

		float startTime = keyframes[0].time;
		float endTime = keyframes[size - 1].time;
		float duration = endTime - startTime;

		if (duration <= 0.0f)
		{
			return 0.0f;
		}

		if (bLooping)
		{
			time = FMod(time - startTime, duration);
			
			if (time < 0.0f)
			{
				time += duration;
			}
			
			time += startTime;
		}
		else
		{
			if (time <= keyframes[0].time)
			{
				time = startTime;
			}

			if (time >= keyframes[size - 1].time)
			{
				time = endTime;
			}
		}

		return time;
	}

	template <>
	float AnimationTrack<float, 1>::cast(float* value)
	{
		return value[0];
	}

	template <>
	Vector3 AnimationTrack<Vector3, 3>::cast(float* value)
	{
		return Vector3(value[0], value[1], value[2]);
	}

	template <>
	Quaternion AnimationTrack<Quaternion, 4>::cast(float* value)
	{
		Quaternion result = Quaternion(value[0], value[1], value[2], value[3]);
		return normalized(result);
	}
}