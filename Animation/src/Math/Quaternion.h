#pragma once

#include "Vector3.h"
#include "Matrix4.h"

namespace Math
{
	struct Quaternion
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			struct
			{
				Vector3 vector;
				float scalar;
			};

			float elements[4];
		};

		static Quaternion Identity;

		inline Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
		inline Quaternion(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}
	};
	
	Quaternion angleAxis(float angle, const Vector3& axis);
	Quaternion fromTo(const Vector3& from, const Vector3& to);
	Vector3 getAxis(const Quaternion& quaternion);
	float getAngle(const Quaternion& quaternion);
	
	Quaternion operator+(const Quaternion& a, const Quaternion& b);
	Quaternion operator-(const Quaternion& a, const Quaternion& b);
	Quaternion operator-(const Quaternion& quaternion);
	Quaternion operator*(const Quaternion& a, float b);
	Quaternion operator/(const Quaternion& a, float b);
	Quaternion operator*(const Quaternion& a, const Quaternion& b);
	Vector3 operator*(const Quaternion& quaternion, const Vector3& vector);
	bool operator==(const Quaternion& a, const Quaternion& b);
	bool operator!=(const Quaternion& a, const Quaternion& b);
	
	bool sameOrientation(const Quaternion& a, const Quaternion& b);
	float dot(const Quaternion& a, const Quaternion& b);
	float lengthSqured(const Quaternion& quaternion);
	float length(const Quaternion& quaternion);
	void normalize(Quaternion& quaternion);
	Quaternion normalized(const Quaternion& quaternion);
	Quaternion conjugate(const Quaternion& quaternion);
	Quaternion inverse(const Quaternion& matrix);
	Quaternion mix(const Quaternion& from, const Quaternion& to, float t);
	Quaternion nlerp(const Quaternion& from, const Quaternion& to, float t);
	Quaternion operator^(const Quaternion& quaternion, float power);
	Quaternion slerp(const Quaternion& start, const Quaternion& end, float t);
	Quaternion lookRotation(const Vector3& direction, const Vector3& up);
	Matrix4 quaternionToMatrix4(const Quaternion& quaternion);
	Quaternion matrix4ToQuaternion(const Matrix4& matrix);
}