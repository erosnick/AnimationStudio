#include "Math.h"
#include "DualQuaternion.h"

namespace Math
{

	DualQuaternion operator+(const DualQuaternion& left, const DualQuaternion& right)
	{
		return DualQuaternion(left.real + right.real, left.dual + right.dual);
	}

	DualQuaternion operator*(const DualQuaternion& dualQuaternion, float scalar)
	{
		return DualQuaternion(dualQuaternion.real * scalar, dualQuaternion.dual * scalar);
	}

	// Start implementing dual quaternion multiplication 
	// by first making sure both dual quaternions are normalized
	// 
	// Remember, multiplication order is left to right. 
	// This is the opposite of matrix and quaternion 
	// multiplication order
	DualQuaternion operator*(const DualQuaternion& left, const DualQuaternion& right)
	{
		DualQuaternion lhs = normalized(left);
		DualQuaternion rhs = normalized(right);

		return DualQuaternion(lhs.real * rhs.real, lhs.real * rhs.dual + lhs.dual * rhs.real);
	}

	bool operator==(const DualQuaternion& left, const DualQuaternion& right)
	{
		return left.real == right.real && left.dual == right.dual;
	}

	bool operator!=(const DualQuaternion& left, const DualQuaternion& right)
	{
		return !(left == right);
	}

	float dot(const DualQuaternion& left, const DualQuaternion& right)
	{
		return dot(left.real, right.real);
	}

	Math::DualQuaternion conjugate(const DualQuaternion& dualQuaternion)
	{
		return DualQuaternion(conjugate(dualQuaternion.real), conjugate(dualQuaternion.dual));
	}

	Math::DualQuaternion normalized(const DualQuaternion& dualQuaternion)
	{
		float magnitudeSquared = dot(dualQuaternion.real, dualQuaternion.real);

		if (magnitudeSquared < Epsilon)
		{
			return DualQuaternion();
		}

		float inverseMagnitude = 1.0f / Math::Sqrt(magnitudeSquared);

		return DualQuaternion(dualQuaternion.real * inverseMagnitude, dualQuaternion.dual * inverseMagnitude);
	}

	void normalize(DualQuaternion& dualQuaternion)
	{
		float magnitudeSquared = dot(dualQuaternion.real, dualQuaternion.real);

		if (magnitudeSquared < Math::Epsilon)
		{
			return;
		}

		float inverseMagnitude = 1.0f / Math::Sqrt(magnitudeSquared);

		dualQuaternion.real = dualQuaternion.real * inverseMagnitude;
	}

	Math::DualQuaternion transformToDualQuaternion(const Transform& transform)
	{
		Quaternion pure = { transform.position.x, transform.position.y, transform.position.z, 0.0f };

		Quaternion real = transform.rotation;
		Quaternion dual = real * pure * 0.5f;

		return DualQuaternion(real, dual);
	}

	Transform dualQuaternionToTransform(const DualQuaternion& dualQuaternion)
	{
		Transform result;

		result.rotation = dualQuaternion.real;
		Quaternion pure = conjugate(dualQuaternion.real) * (dualQuaternion.dual * 2.0f);
		result.position = Vector3(pure.x, pure.y, pure.z);

		return result;
	}

	Math::Vector3 transformVector(const DualQuaternion& dualQuaternion, const Vector3& vector)
	{
		return dualQuaternion.real * vector;
	}

	Math::Vector3 transformPoint(const DualQuaternion& dualQuaternion, const Vector3& point)
	{
		Quaternion pure = conjugate(dualQuaternion.real) * (dualQuaternion.dual * 2.0f);
		Vector3 translation = Vector3(pure.x, pure.y, pure.z);
		
		return dualQuaternion.real * point + translation;
	}
}