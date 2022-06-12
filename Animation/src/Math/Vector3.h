#pragma once

#include <iostream>
#include <sstream>

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

		inline float operator[](int index) { return elements[index]; }

		inline Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
		inline Vector3(float value) : x(value), y(value), z(value) {}
		inline Vector3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {}
		inline Vector3(float* data) : x(data[0]), y(data[1]), z(data[3]) {}

		inline std::string toString() const
		{
			std::stringstream ss;
			ss << "Matrix4(" << x << ", " << y << ", " << z << ", " << std::endl;

			return ss.str();
		}

		static Vector3 Zero;
		static Vector3 One;
		static Vector3 X;
		static Vector3 Y;
		static Vector3 Z;
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