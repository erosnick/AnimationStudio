#pragma once

namespace Math
{
	struct Vector3
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};
			float elements[3];
		};

		inline Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
		inline Vector3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}
		inline Vector3(float* data) : x(data[0]), y(data[1]), z(data[3]) {}
	};

	Vector3 operator+(const Vector3& v1, const Vector3& v2);
	Vector3 operator-(const Vector3& v1, const Vector3& v2);
	Vector3 operator*(const Vector3& v, float scalefactor);
	Vector3 operator*(float scalefactor, const Vector3& v);
	Vector3 operator*(const Vector3& v1, const Vector3& v2);
	bool operator==(const Vector3& v1, const Vector3& v2);
	bool operator!=(const Vector3& v1, const Vector3& v2);

	float dot(const Vector3& v1, const Vector3& v2);
	float lengthSqure(const Vector3& v);
	float length(const Vector3& v);
	void normalize(Vector3& v);
	Vector3 normalized(const Vector3& v);
	float angle(const Vector3& v1, const Vector3& v2);
	Vector3 project(const Vector3& a, const Vector3& b);
	Vector3 reject(const Vector3& a, const Vector3& b);
	Vector3 reflect(const Vector3& a, const Vector3& b);
	Vector3 cross(const Vector3& a, const Vector3& b);
	Vector3 lerp(const Vector3& source, const Vector3& target, float t);
	Vector3 slerp(const Vector3& source, const Vector3& target, float t);
	Vector3 nlerp(const Vector3& source, const Vector3& target, float t);
}