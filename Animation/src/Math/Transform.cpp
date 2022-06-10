#include "Transform.h"
#include "Math.h"

namespace Math
{
	Transform combine(const Transform& a, const Transform& b)
	{
		Transform result;

		result.scale = a.scale * b.scale;
		result.rotation = a.rotation * b.rotation;

		result.position = a.rotation * (a.scale * b.position);
		result.position = a.position + result.position;

		return result;
	}

	Transform inverse(const Transform& transform)
	{
		Transform inversed;

		inversed.rotation = inverse(transform.rotation);
		inversed.scale.x = FastAbs(transform.scale.x) < Epsilon ? 0.0f : 1.0f / transform.scale.x;
		inversed.scale.y = FastAbs(transform.scale.y) < Epsilon ? 0.0f : 1.0f / transform.scale.y;
		inversed.scale.z = FastAbs(transform.scale.z) < Epsilon ? 0.0f : 1.0f / transform.scale.z;

		Vector3 inversedTranslation = transform.position * -1.0f;

		inversed.position = inversed.rotation * (inversed.scale * inversedTranslation);

		return inversed;
	}

	Transform lerp(const Transform& source, const Transform& target, float t)
	{
		Quaternion targetRotation = target.rotation;

		if (dot(source.rotation, targetRotation) < 0.0f)
		{
			targetRotation = -targetRotation;
		}

		return Transform(
			lerp(source.position, target.position, t),
			nlerp(source.rotation, targetRotation, t),
			lerp(source.scale, target.scale, t));
	}

	Matrix4 transformToMatrix4(const Transform& transform)
	{
		// First, extract the rotation basis of the transform
		Vector3 x = transform.rotation * Vector3::X;
		Vector3 y = transform.rotation * Vector3::Y;
		Vector3 z = transform.rotation * Vector3::Z;

		// Next, scale the basis vectors
		x = x * transform.scale.x;
		y = y * transform.scale.y;
		z = z * transform.scale.z;

		// Extract the position of the transform
		Vector3 position = transform.position;

		// Create matrix
		return Matrix4(x.x,		   x.y,		   x.z,		   0.0f,  // X basis (& Scale)
					   y.x,		   y.y,		   y.z,		   0.0f,  // Y basis (& Scale)
					   z.x,		   z.y,		   z.z,		   0.0f,  // Z basis (& Scale)
					   position.x, position.y, position.z, 1.0f); // Position
	}

	Transform matrix4ToTransform(const Matrix4& matrtix)
	{
		Transform result;

		result.position = Vector3(matrtix[12], matrtix[13], matrtix[14]);
		result.rotation = matrix4ToQuaternion(matrtix);

		Matrix4 rotationScaleMatrix(
			matrtix[0], matrtix[1], matrtix[2], 0.0f,
			matrtix[4], matrtix[5], matrtix[6], 0.0f,
			matrtix[8], matrtix[9], matrtix[10], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		Matrix4 inversedRotationMatrix = quaternionToMatrix4(inverse(result.rotation));
		Matrix4 scaleSkewMatrix = rotationScaleMatrix * inversedRotationMatrix;

		result.scale = Vector3(scaleSkewMatrix[0],
							   scaleSkewMatrix[5],
							   scaleSkewMatrix[10]);

		return result;
	}
	
	Vector3 transformPoint(const Transform& transform, const Vector3& point)
	{
		return transform.rotation * (transform.scale * point) + transform.position;
	}
	
	Vector3 transformVector(const Transform& transform, const Vector3& vector)
	{
		return transform.rotation * (transform.scale * vector);
	}
}