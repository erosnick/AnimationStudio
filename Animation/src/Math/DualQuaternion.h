#pragma once

#include "Vector3.h"
#include "Transform.h"
#include "Quaternion.h"

namespace Math
{
	struct DualQuaternion
	{
		union
		{
			struct
			{
				// The real part of a dual quaternion holds rotation data, 
				// and the dual part holds position data.
				Quaternion real;
				Quaternion dual;
			};
			float data[8];
		};
		
		inline DualQuaternion() : real(0.0f, 0.0f, 0.0f, 1.0f), dual(0.0f, 0.0f, 0.0f, 0.0f)
		{}

		inline DualQuaternion(const Quaternion& inReal, const Quaternion& inDual) : real(inReal), dual(inDual)
		{}
	};
	
	DualQuaternion operator+(const DualQuaternion& left, const DualQuaternion& right);
	DualQuaternion operator*(const DualQuaternion& dualQuaternion, float scalar);

	// Multiplication order is left to right
	// This is the OPPOSITE of matrices and quaternions
	DualQuaternion operator*(const DualQuaternion& left, const DualQuaternion& right);
	bool operator==(const DualQuaternion& left, const DualQuaternion& right);
	bool operator!=(const DualQuaternion& left, const DualQuaternion& right);

	float dot(const DualQuaternion& left, const DualQuaternion& right);
	DualQuaternion conjugate(const DualQuaternion& dualQuaternion);
	DualQuaternion normalized(const DualQuaternion &dualQuaternion);
	void normalize(DualQuaternion& dualQuaternion);

	DualQuaternion transformToDualQuaternion(const Transform& transform);
	Transform dualQuaternionToTransform(const DualQuaternion& dualQuaternion);

	Vector3 transformVector(const DualQuaternion& dualQuaternion, const Vector3& vector);
	Vector3 transformPoint(const DualQuaternion& dualQuaternion, const Vector3& point);
}