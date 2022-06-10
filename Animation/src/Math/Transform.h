#pragma once

#include "Vector3.h"
#include "Quaternion.h"

namespace Math
{
	struct Transform
	{
		Vector3 position;
		Quaternion rotation;
		Vector3 scale;

		inline Transform() : position(Vector3::Zero), rotation(Quaternion::Identity), scale(Vector3::One) {}
		inline Transform(const Vector3& inPosition, const Quaternion& inRotation, const Vector3& inScale)
		: position(inPosition), rotation(inRotation), scale(inScale) {}
	};

	Transform combine(const Transform& a, const Transform& b);
	Transform inverse(const Transform& transform);
	Transform lerp(const Transform& source, const Transform& target, float t);
	Matrix4 transformToMatrix4(const Transform& transform);
	Transform matrix4ToTransform(const Matrix4& matrtix);
	Vector3 transformPoint(const Transform& transform, const Vector3& point);
	Vector3 transformVector(const Transform& transform, const Vector3& vector);
}