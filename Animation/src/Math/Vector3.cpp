#include "Vector3.h"
#include "Math.h"
#include <cmath>

namespace Math
{
	// TODO: 使用elements[0]的形式访问
	Vector3 Vector3::Zero = Vector3();
	Vector3 Vector3::One = Vector3(1.0f);
	Vector3 Vector3::X = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 Vector3::Y = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 Vector3::Z = Vector3(0.0f, 0.0f, 1.0f);

	
	Math::Vector3 operator+(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1.x + v2.x, v2.y + v2.y, v1.z + v2.z);
	}

	Math::Vector3 operator-(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1.x - v2.x, v2.y - v2.y, v1.z - v2.z);
	}

	Math::Vector3 operator*(const Vector3& v, float scalefactor)
	{
		return Vector3(v.x * scalefactor, v.y * scalefactor, v.z * scalefactor);
	}

	Math::Vector3 operator*(float scalefactor, const Vector3& v)
	{
		return v * scalefactor;
	}

	Vector3 operator*(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}

	bool operator==(const Vector3& v1, const Vector3& v2)
	{
		Vector3 diff(v1 - v2);
		return lengthSqure(diff) < Epsilon;
	}

	bool operator!=(const Vector3& v1, const Vector3& v2)
	{
		return !(v1 == v2);
	}

	float dot(const Vector3& v1, const Vector3& v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
	}

	float lengthSqure(const Vector3& v)
	{
		return (v.x * v.x + v.y * v.y + v.z * v.z);
	}

	float length(const Vector3& v)
	{
		float squredLength = lengthSqure(v);

		if (squredLength < Epsilon)
		{
			return 0.0f;
		}

		return Sqrt(squredLength);
	}

	void normalize(Vector3& v)
	{
		float squredLength = lengthSqure(v);

		if (squredLength < Epsilon)
		{
			return;
		}

		float inverseLength = 1.0f / Sqrt(squredLength);

		v.x *= inverseLength;
		v.y *= inverseLength;
		v.z *= inverseLength;
	}

	Vector3 normalized(const Vector3& v)
	{
		float squredLength = lengthSqure(v);

		if (squredLength < Epsilon)
		{
			return v;
		}

		float inverseLength = 1.0f / Sqrt(squredLength);

		return Vector3(v.x * inverseLength,
			v.y * inverseLength,
			v.z * inverseLength);
	}

	float angle(const Vector3& v1, const Vector3& v2)
	{
		float squredLength1 = lengthSqure(v1);
		float squredLength2 = lengthSqure(v2);

		if (squredLength1 < Epsilon || squredLength2 < Epsilon)
		{
			return 0.0f;
		}

		float dotValue = dot(v1, v2);
		float length = Sqrt(squredLength1 * squredLength2);
		return ACos(dotValue / length);
	}

	Vector3 project(const Vector3& a, const Vector3& b)
	{
		float lengthB = length(b);

		if (lengthB < Epsilon)
		{
			return Vector3();
		}

		float scale = dot(a, b) / lengthB;

		return b * scale;
	}

	Vector3 reject(const Vector3& a, const Vector3& b)
	{
		Vector3 projection = project(a, b);
		return a - projection;
	}

	Vector3 reflect(const Vector3& a, const Vector3& b)
	{
		float lengthB = length(b);

		if (lengthB < Epsilon)
		{
			return Vector3();
		}

		float scale = dot(a, b) / lengthB;
		Vector3 projection = b * (scale * 2);
		return a - projection;
	}

	Vector3 cross(const Vector3& a, const Vector3& b)
	{
		return Vector3(a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
	}

	Vector3 lerp(const Vector3& source, const Vector3& target, float t)
	{
		// (1 - t ) * source + t * target;
		return Vector3(source.x + (target.x - source.x) * t,
					   source.y + (target.y - source.y) * t,
					   source.z + (target.z - source.z) * t);
	}

	Vector3 slerp(const Vector3& source, const Vector3& target, float t)
	{
		if (t < 0.01f)
		{
			return lerp(source, target, t);
		}

		Vector3 from = normalized(source);
		Vector3 to = normalized(target);

		float theta = angle(from, to);
		float sinTheta = Sin(theta);

		float a = Sin((1.0f - t) * theta) / sinTheta;
		float b = Sin(t * theta) / sinTheta;

		return from * a + to * b;
	}

	Vector3 nlerp(const Vector3& source, const Vector3& target, float t)
	{
		Vector3 linear(source.x + (target.x - source.x) * t,
					   source.y + (target.y - source.y) * t,
					   source.z + (target.z - source.z) * t);

		return normalized(linear);
	}
}
