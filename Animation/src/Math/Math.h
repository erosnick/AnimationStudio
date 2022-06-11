#pragma once

#include <cmath>
#include <algorithm>

namespace Math
{
	constexpr float Epsilon = 0.000001f;
	constexpr float RadianToDegree = 57.2958f;
	constexpr float DegreeToRadian = 0.0174533f;
	constexpr float PI = 3.14159265359f;
	constexpr float PI_2 = 6.28318530718f;
	constexpr float PI_DIV_2 = 1.570796326795f;
	constexpr float PI_DIV_3 = 1.047197551197f;
	constexpr float PI_DIV_4 = 0.785398163398f;
	constexpr float PI_DIV_5 = 0.628318530718f;
	constexpr float PI_DIV_6 = 0.523598775598f;
	constexpr float PI_DIV_7 = 0.448798950513f;
	constexpr float PI_DIV_8 = 0.392699081699f;
	constexpr float Degree_30 = 30.0f;
	constexpr float Degree_45 = 45.0f;
	constexpr float Degree_60 = 60.0f;
	constexpr float Degree_90 = 90.0f;
	constexpr float Degree_180 = 180.0f;
	constexpr float Degree_360 = 360.0f;
	
	bool FloatEqual(float a, float b);
	bool FloatNotEqual(float a, float b);
	
	inline float Sin(float angle) { return std::sinf(angle); }
	inline float Cos(float angle) { return std::cosf(angle); }
	inline float Sqrt(float value) { return std::sqrtf(value); }
	inline float ACos(float value) { return std::acosf(value); }
	inline float FastAbs(float value) { return std::fabsf(value); }
	inline float FMod(float x, float y) { return std::fmodf(x, y); }

	template <typename T>
	inline T Min(T x, T y) { return std::min(x, y); }
	template <typename T>	
	inline T Max(T x, T y) { return std::max(x, y); }
}