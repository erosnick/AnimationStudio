#pragma once

#include "Vector3.h"
#include "Vector4.h"

#include <iostream>
#include <sstream>

namespace Math
{
	struct Matrix4 
	{
		union
		{
			float elements[16];
			
			struct
			{
				Vector4 right;
				Vector4 up;
				Vector4 forward;
				Vector4 position;
			};

			struct
			{
				// Row1   Row2      Row3      Row4
				float xx; float xy; float xz; float xw;
				float yx; float yy; float yz; float yw;
				float zx; float zy; float zz; float zw;
				float tx; float ty; float tz; float tw;
			};

			struct
			{
				// Row1    Row2       Row3       Row4
				float m00; float m10; float m20; float m30;
				float m01; float m11; float m21; float m31;
				float m02; float m12; float m22; float m32;
				float m03; float m13; float m23; float m33;
			};
		};

		inline Matrix4() :
			m00(1.0f), m10(0.0f), m20(0.0f), m30(0.0f),
			m01(0.0f), m11(1.0f), m21(0.0f), m31(0.0f),
			m02(0.0f), m12(0.0f), m22(1.0f), m32(0.0f),
			m03(0.0f), m13(0.0f), m23(0.0f), m33(1.0f) {}
		
		inline Matrix4(float* data) :
			m00(data[0]),  m10(data[1]),  m20(data[2]),  m30(data[3]),
			m01(data[4]),  m11(data[5]),  m21(data[6]),  m31(data[7]),
			m02(data[8]),  m12(data[9]),  m22(data[10]), m32(data[11]),
			m03(data[12]), m13(data[13]), m23(data[14]), m33(data[15]) {}

		inline Matrix4(float inM00, float inM10, float inM20, float inM30,
				       float inM01, float inM11, float inM21, float inM31,
				       float inM02, float inM12, float inM22, float inM32,
					   float inM03, float inM13, float inM23, float inM33) :
					   m00(inM00), m10(inM10), m20(inM20), m30(inM30),
					   m01(inM01), m11(inM11), m21(inM21), m31(inM31),
					   m02(inM02), m12(inM12), m22(inM22), m32(inM32),
					   m03(inM03), m13(inM13), m23(inM23), m33(inM33) {}

		// TODO: Performance issue
		inline float operator[](int index) const { return elements[index]; }

		inline std::string toString() const
		{
			std::stringstream ss;
			ss << "Matrix4(" << m00 << ", " << m10 << ", " << m20 << ", " << m30 << ", " << std::endl;
			ss << "        " << m01 << ", " << m11 << ", " << m21 << ", " << m31 << ", " << std::endl;
			ss << "        " << m02 << ", " << m12 << ", " << m22 << ", " << m32 << ", " << std::endl;
			ss << "        " << m03 << ", " << m13 << ", " << m23 << ", " << m33 << ", " << std::endl;
			ss << std::endl;

			return ss.str();
		}

		static Matrix4 Identity;
	};

	bool operator==(const Matrix4& a, const Matrix4& b);
	bool operator!=(const Matrix4& a, const Matrix4& b);
	Matrix4 operator+(const Matrix4& a, const Matrix4& b);
	Matrix4 operator*(const Matrix4& matrix, float scalar);
	Matrix4 operator/(const Matrix4& matrix, float scalar);
	Matrix4 operator*(const Matrix4& a, const Matrix4& b);
	Vector4 operator*(const Matrix4& matrix, const Vector4& vector);
	
	Vector3 transformVector(const Matrix4& matrix, const Vector3& vector);
	Vector3 transformPoint(const Matrix4& matrix, const Vector3& point);
	Vector3 transformPoint(const Matrix4& matrix, const Vector3& point, float& w);
	
	void transpose(Matrix4& matrix);
	Matrix4 transposed(const Matrix4& matrix);
	float determinant(const Matrix4& matrix);
	Matrix4 adjugate(const Matrix4& matrix);
	Matrix4 inverse(const Matrix4& matrix);
	void invert(Matrix4& matrix);
	Matrix4 frustum(float left, float right, float bottom, float top, float znear, float zfar);
	Matrix4 perspective(float fov, float aspect, float znear, float zfar);
	Matrix4 ortho(float left, float right, float bottom, float top, float znear, float zfar);
	Matrix4 lookAt(const Vector3& position, const Vector3& target, const Vector3& worldUp);
}