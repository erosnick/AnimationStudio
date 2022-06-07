#pragma once

#include <Math/Vector3.h>
#include <Math/Quaternion.h>

using namespace Math;

namespace AnimationTrackHelpers
{
	inline float interpolate(float source, float target, float t)
	{
		return source + (target - source) * t;
	}

	inline Vector3 interpolate(const Vector3& source, const Vector3& target, float t)
	{
		return lerp(source, target, t);
	}

	inline Quaternion interpolate(const Quaternion& source, const Quaternion& target, float t)
	{
		Quaternion result = mix(source, target, t);

		// Neighborhood
		if (dot(source, target) < 0.0f)
		{
			result = mix(source, -target, t);
		}

		// nlerp, not slerp
		return normalized(result);
	}

	inline float adjustHermiteResult(float value)
	{
		return value;
	}

	inline Vector3 adjustHermiteResult(const Vector3& value)
	{
		return value;
	}

	inline Quaternion adjustHermiteResult(const Quaternion& value)
	{
		return normalized(value);
	}

	inline void neighborhood(const float& a, const float& b) {}
	inline void neighborhood(const Vector3& a, const Vector3& b) {}

	inline void Neighborhood(const Quaternion& a, Quaternion& b)
	{
		if (dot(a, b) < 0.0f) {
			b = -b;
		}
	}
}