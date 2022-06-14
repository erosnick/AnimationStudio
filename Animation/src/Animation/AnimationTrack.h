#pragma once

#include <cstdint>
#include <vector>

#include "AnimationKeyFrame.h"
#include <Math/Interpolation.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

using namespace Math;

namespace Animation
{
	template <typename T, int32_t N>
	class AnimationTrack
	{
	public:
		AnimationTrack();
		
		void resize(uint32_t size);
		
		uint32_t frameCount() const;
		Interpolation getInterpolation() const;
		
		void setInterpolation(Interpolation inInterpolation);
		
		float getStartTime() const;
		float getEndTime() const;
		
		T sample(float time, bool bLooping);
		
		AnimationKeyFrame<N>& operator[](uint32_t index);
	protected:
		T sampleConstant(float time, bool bLooping);
		T sampleLinear(float time, bool bLooping);
		T sampleCubic(float time, bool bLooping);
		T hermite(float time, const T& p1, const T& s1, const T& p2, const T& s2);
		
		virtual int32_t frameIndex(float time, bool bLooping);
		float adjustTimeToFitTrack(float time, bool bLooping);

		T cast(float* value);	// Will be specialized
	protected:
		std::vector<AnimationKeyFrame<N>> keyframes;
		Interpolation interpolation;
	};

	using ScalarTrack = AnimationTrack<float, 1>;
	using VectorTrack = AnimationTrack<Vector3, 3>;
	using QuaternionTrack = AnimationTrack<Quaternion, 4>;
}