#include "Quaternion.h"
#include "Math.h"

namespace Math
{
	Quaternion Quaternion::Identity = Quaternion();
	
	Quaternion angleAxis(float angle, const Vector3& axis)
	{
		Vector3 norm = normalized(axis);
		float sin = Sin(angle * 0.5f);

		return Quaternion(norm.x * sin,
			norm.y * sin,
			norm.z * sin,
			Cos(angle * 0.5f));
	}

	Quaternion fromTo(const Vector3& from, const Vector3& to)
	{
		Vector3 f = normalized(from);
		Vector3 t = normalized(to);

		if (f == t)
		{
			return Quaternion();
		}
		else if (f == t * -1.0f)
		{
			Vector3 ortho = Vector3(1.0f, 0.0f, 0.0f);

			if (FastAbs(f.y) < FastAbs(f.x))
			{
				ortho = Vector3(0.0f, 1.0f, 0.0f);
			}

			if (FastAbs(f.z) < FastAbs(f.y) && FastAbs(f.z) < FastAbs(f.x))
			{
				ortho = Vector3(0.0f, 0.0f, 1.0f);
			}

			Vector3 axis = normalized(cross(f, ortho));
			return Quaternion(axis.x, axis.y, axis.z, 0.0f);
		}

		Vector3 half = normalized(f + t);
		Vector3 axis = cross(f, half);
		return Quaternion(axis.x, axis.y, axis.z, dot(f, half));
	}

	Vector3 getAxis(const Quaternion& quaternion)
	{
		return normalized(Vector3(quaternion.x, quaternion.y, quaternion.z));
	}

	float getAngle(const Quaternion& quaternion)
	{
		return 2.0f * ACos(quaternion.w);
	}

	Quaternion operator+(const Quaternion& a, const Quaternion& b)
	{
		return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	Quaternion operator-(const Quaternion& a, const Quaternion& b)
	{
		return Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	Quaternion operator-(const Quaternion& quaternion)
	{
		return Quaternion(-quaternion.x, -quaternion.y, -quaternion.z, -quaternion.w);
	}

	Quaternion operator*(const Quaternion& a, float b)
	{
		return Quaternion(a.x * b, a.y * b, a.z * b, a.w * b);
	}

	Quaternion operator/(const Quaternion& a, float b)
	{
		float inversed = 1.0f / b;
		return a * inversed;
	}
	
	Quaternion operator*(const Quaternion& a, const Quaternion& b)
	{
		//Quaternion result;
		//result.scalar = b.scalar * a.scalar - dot(b.vector, a.vector);
		//result.vector = (a.vector * b.scalar) + (b.vector * a.scalar) + cross(b.vector, a.vector);
		//return result;
		
		return Quaternion(a.w * b.x + a.x * b.w + a.y * b.z - a.y * b.z,
						  a.w * b.y + a.y * b.w + a.z * b.x - a.z * b.x,
						  a.w * b.z + a.z * b.w + a.x * b.y - a.x * b.y,
						  a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
	}

	Vector3 operator*(const Quaternion& quaternion, const Vector3& vector)
	{
		return quaternion.vector * 2.0f * dot(quaternion.vector, vector) +
			   vector * (quaternion.scalar * quaternion.scalar - 
			   dot(quaternion.vector, quaternion.vector)) +
			   cross(quaternion.vector, vector) * 2.0f * quaternion.scalar;
	}

	bool operator==(const Quaternion& a, const Quaternion& b)
	{
		return (FastAbs(a.x - b.x) <= Epsilon &&
				FastAbs(a.y - b.y) <= Epsilon &&
				FastAbs(a.z - b.z) <= Epsilon &&
				FastAbs(a.w - b.w) <= Epsilon);
	}
	
	bool operator!=(const Quaternion& a, const Quaternion& b)
	{
		return !(a == b);
	}

	bool sameOrientation(const Quaternion& a, const Quaternion& b)
	{
		return (FastAbs(a.x - b.x) <= Epsilon &&
				FastAbs(a.y - b.y) <= Epsilon &&
				FastAbs(a.z - b.z) <= Epsilon &&
				FastAbs(a.w - b.w) <= Epsilon) ||
			   (FastAbs(a.x + b.x) <= Epsilon &&
				FastAbs(a.y + b.y) <= Epsilon &&
				FastAbs(a.z + b.z) <= Epsilon &&
				FastAbs(a.w + b.w) <= Epsilon);
	}

	float dot(const Quaternion& a, const Quaternion& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	float lengthSqured(const Quaternion& quaternion)
	{
		return dot(quaternion, quaternion);
	}

	float length(const Quaternion& quaternion)
	{
		return Sqrt(lengthSqured(quaternion));
	}

	void normalize(Quaternion& quaternion)
	{
		float squredLength = lengthSqured(quaternion);

		float inversedLength = 1.0f / Sqrt(squredLength);

		quaternion.x *= inversedLength;
		quaternion.y *= inversedLength;
		quaternion.z *= inversedLength;
		quaternion.w *= inversedLength;
	}

	Quaternion normalized(const Quaternion& quaternion)
	{
		Quaternion result = quaternion;
		normalize(result);
		return result;
	}

	Quaternion conjugate(const Quaternion& quaternion)
	{
		return Quaternion(-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w);
	}

	Quaternion inverse(const Quaternion& matrix)
	{
		return conjugate(matrix) / lengthSqured(matrix);
	}

	Quaternion mix(const Quaternion& from, const Quaternion& to, float t)
	{
		return from * (1.0f - t) + to * t;
	}

	Quaternion nlerp(const Quaternion& from, const Quaternion& to, float t)
	{
		return normalized(mix(from, to, t));
	}

	Quaternion operator^(const Quaternion& quaternion, float power)
	{
		float angle = 2.0f * ACos(quaternion.scalar);
		Vector3 axis = normalized(quaternion.vector);
		float halfCos = Cos(power * angle * 0.5f);
		float halfSin = Sin(power * angle * 0.5f);
		
		return Quaternion(axis.x * halfSin,
						  axis.y * halfSin,
						  axis.z * halfSin,
						  halfCos);
	}

	Quaternion slerp(const Quaternion& start, const Quaternion& end, float t)
	{
		if (FastAbs(dot(start, end)) > 1.0f - Epsilon) {
			return nlerp(start, end, t);
		}
		
		Quaternion delta = inverse(start) * end;
		return normalized((delta ^ t) * start);
	}

	Quaternion lookRotation(const Vector3& direction, const Vector3& up)
	{
		// Find orthonormal basis vectors
		Vector3 forward = normalized(direction); // Object Forward
		Vector3 realUp = normalized(up); // Desired Up
		Vector3 right = cross(realUp, forward); // Object Right
		realUp = cross(forward, right); // Object Up
		
		// From world forward to object forward
		Quaternion worldToObject = fromTo(Vector3(0, 0, 1), forward);
		
		// what direction is the new object up?
		Vector3 objectUp = worldToObject * Vector3(0, 1, 0);
		
		// From object up to desired up
		Quaternion objectToDesired = fromTo(objectUp, realUp);
		
		// Rotate to forward direction first
		// then twist to correct up
		Quaternion result = worldToObject * objectToDesired;
		
		// Don't forget to normalize the result
		return normalized(result);
	}

	Matrix4 quaternionToMatrix4(const Quaternion& quaternion)
	{
		Vector3 right = quaternion * Vector3(1, 0, 0);
		Vector3 up = quaternion * Vector3(0, 1, 0);
		Vector3 forward = quaternion * Vector3(0, 0, 1);
		
		return Matrix4(right.x, right.y, right.z, 0.0f,
					   up.x, up.y, up.z, 0.0f,
					   forward.x, forward.y, forward.z, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);
	}

	Quaternion matrix4ToQuaternion(const Matrix4& matrix)
	{
		Vector3 up = normalized(Vector3(matrix.up.x, matrix.up.y, matrix.up.z));
		Vector3 forward = normalized(Vector3(matrix.forward.x, matrix.forward.y, matrix.forward.z));
		Vector3 right = cross(up, forward);
		up = cross(forward, right);
		
		return lookRotation(forward, up);
	}
}