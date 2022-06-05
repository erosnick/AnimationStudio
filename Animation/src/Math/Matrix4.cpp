#include "Matrix4.h"
#include "Math.h"

#include <iostream>
#include <utility>

namespace Math
{
#define Matrix4Dot(aRow, bColumn) \
	a[0 * 4 + aRow] * b[bColumn * 4 + 0] + \
	a[1 * 4 + aRow] * b[bColumn * 4 + 1] + \
	a[2 * 4 + aRow] * b[bColumn * 4 + 2] + \
	a[3 * 4 + aRow] * b[bColumn * 4 + 3]

#define Matrix4Vector4Dot(row, x, y, z, w) \
	x * matrix[0 * 4 + row] + \
	y * matrix[1 * 4 + row] + \
	z * matrix[2 * 4 + row] + \
	w * matrix[3 * 4 + row]

#define Matrix4Minor(elements, column0, column1, column2, row0, row1, row2) \
	(elements[column0 * 4 + row0]  * (elements[column1 * 4 + row1]  *  elements[column2 * 4 + row2] -  elements[column1 * 4 + row2] * \
	 elements[column2 * 4 + row1]) -  elements[column1 * 4 + row0]  * (elements[column0 * 4 + row1] *  elements[column2 * 4 + row2] - \
	 elements[column0 * 4 + row2]  *  elements[column2 * 4 + row1]) +  elements[column2 * 4 + row0] * (elements[column0 * 4 + row1] * \
	 elements[column1 * 4 + row2]  -  elements[column0 * 4 + row2]  *  elements[column1 * 4 + row1]))

	Matrix4 Matrix4::Identity = Matrix4();
	
	// TODO: 使用m[0][0]的形式访问
	// TODO: 循环展开性能测试
	bool operator==(const Matrix4& a, const Matrix4& b)
	{
		//return a.m00 == b.m00 && a.m10 == b.m10 && a.m20 == b.m20 && a.m30 == b.m30 &&
		//		 a.m01 == b.m01 && a.m11 == b.m11 && a.m21 == b.m21 && a.m31 == b.m31 &&
		for (auto i = 0; i < 16; i++)
		{
			if (!FloatEqual(a.elements[i], b.elements[i]))
			{
				return false;
			}
		}

		return true;
	}

	bool operator!=(const Matrix4& a, const Matrix4& b)
	{
		return !(a == b);
	}

	Matrix4 operator+(const Matrix4& a, const Matrix4& b)
	{
		Matrix4 result;
		for (auto i = 0; i < 16; i++)
		{
			result.elements[i] = a.elements[i] + b.elements[i];
		}
		return result;
	}

	Matrix4 operator*(const Matrix4& a, float scaleFactor)
	{
		Matrix4 result;
		for (auto i = 0; i < 16; i++)
		{
			result.elements[i] = a.elements[i] * scaleFactor;
		}
		return result;
	}

	Matrix4 operator/(const Matrix4& a, float scaleFactor)
	{
		return a * (1.0f / scaleFactor);
	}

	Matrix4 operator*(const Matrix4& a, const Matrix4& b)
	{
		// Github Copilot
		//Matrix4 result;
		//for (auto i = 0; i < 4; i++)
		//{
		//	for (auto j = 0; j < 4; j++)
		//	{
		//		float sum = 0.0f;
		//		for (auto k = 0; k < 4; k++)
		//		{
		//			sum += a.elements[i * 4 + k] * b.elements[k * 4 + j];
		//		}
		//		result.elements[i * 4 + j] = sum;
		//	}
		//}
		//return result;
		return Matrix4(Matrix4Dot(0, 0), Matrix4Dot(1, 0), Matrix4Dot(2, 0), Matrix4Dot(3, 0),	// Column 0
			Matrix4Dot(0, 1), Matrix4Dot(1, 1), Matrix4Dot(2, 1), Matrix4Dot(3, 1),	// Column 1
			Matrix4Dot(0, 2), Matrix4Dot(1, 2), Matrix4Dot(2, 2), Matrix4Dot(3, 2),	// Column 2
			Matrix4Dot(0, 3), Matrix4Dot(1, 3), Matrix4Dot(2, 3), Matrix4Dot(3, 3));	// Column 3
	}

	Vector4 operator*(const Matrix4& matrix, const Vector4& vector)
	{
		return Vector4(Matrix4Vector4Dot(0, vector.x, vector.y, vector.z, vector.w),
			Matrix4Vector4Dot(1, vector.x, vector.y, vector.z, vector.w),
			Matrix4Vector4Dot(2, vector.x, vector.y, vector.z, vector.w),
			Matrix4Vector4Dot(3, vector.x, vector.y, vector.z, vector.w));
	}

	Vector3 transformVector(const Matrix4& matrix, const Vector3& vector)
	{
		return Vector3(Matrix4Vector4Dot(0, vector.x, vector.y, vector.z, 0.0f),
			Matrix4Vector4Dot(1, vector.x, vector.y, vector.z, 0.0f),
			Matrix4Vector4Dot(2, vector.x, vector.y, vector.z, 0.0f));
	}

	Vector3 transformPoint(const Matrix4& matrix, const Vector3& point)
	{
		return Vector3(Matrix4Vector4Dot(0, point.x, point.y, point.z, 1.0f),
					   Matrix4Vector4Dot(1, point.x, point.y, point.z, 1.0f),
					   Matrix4Vector4Dot(2, point.x, point.y, point.z, 1.0f));
	}

	Vector3 transformPoint(const Matrix4& matrix, const Vector3& point, float& w)
	{
		float tempW = w;
		w = Matrix4Vector4Dot(3, point.x, point.y, point.z, tempW);

		return Vector3(Matrix4Vector4Dot(0, point.x, point.y, point.z, tempW),
					   Matrix4Vector4Dot(1, point.x, point.y, point.z, tempW),
					   Matrix4Vector4Dot(2, point.x, point.y, point.z, tempW));
	}

	void transpose(Matrix4& matrix)
	{
		std::swap(matrix.yx, matrix.xy);
		std::swap(matrix.zx, matrix.xz);
		std::swap(matrix.tx, matrix.xw);
		std::swap(matrix.zy, matrix.yz);
		std::swap(matrix.ty, matrix.yw);
		std::swap(matrix.tz, matrix.zw);
	}

	Matrix4 transposed(const Matrix4& matrix)
	{
		return Matrix4(
			matrix.xx, matrix.yx, matrix.zx, matrix.tx,
			matrix.xy, matrix.yy, matrix.zy, matrix.ty,
			matrix.xz, matrix.yz, matrix.zz, matrix.tz,
			matrix.xw, matrix.yw, matrix.zw, matrix.tw
		);
	}

	float determinant(const Matrix4& matrix)
	{
		// Github Copilot
		//return matrix.xx * (matrix.yy * matrix.zz - matrix.yz * matrix.zy) -
		//	   matrix.xy * (matrix.yx * matrix.zz - matrix.zx * matrix.zy) +
		//	   matrix.xz * (matrix.yx * matrix.zy - matrix.zx * matrix.yy) +
		//	   matrix.xw * (matrix.yx * matrix.yz - matrix.zx * matrix.zy) -
		//	   matrix.yx * (matrix.xx * matrix.zz - matrix.xz * matrix.zx) +
		//	   matrix.yy * (matrix.xx * matrix.zy - matrix.xz * matrix.zx) -
		//	   matrix.yz * (matrix.xx * matrix.zy - matrix.xz * matrix.zx) +
		//	   matrix.yw * (matrix.xx * matrix.yz - matrix.xz * matrix.zx) -
		//	   matrix.zx * (matrix.xy * matrix.yz - matrix.yy * matrix.yz) +
		//	   matrix.zy * (matrix.xy * matrix.zx - matrix.yx * matrix.yz) -
		//	   matrix.zz * (matrix.xy * matrix.zx - matrix.yx * matrix.yz) +
		//	   matrix.zw * (matrix.xy * matrix.zx - matrix.yx * matrix.yz) -
		//	   matrix.xw * (matrix.xy * matrix.yw - matrix.yy * matrix.yw) +
		//	   matrix.yw * (matrix.xy * matrix.xw - matrix.yx * matrix.yw) -
		//	   matrix.zw * (matrix.xy * matrix.xw - matrix.yx * matrix.yw) +
		//	   matrix.tw * (matrix.xy * matrix.xw - matrix.yx * matrix.yw);
		return matrix.elements[0]  * Matrix4Minor(matrix.elements, 1, 2, 3, 1, 2, 3)
			 - matrix.elements[4]  * Matrix4Minor(matrix.elements, 0, 2, 3, 1, 2, 3)
			 + matrix.elements[8]  * Matrix4Minor(matrix.elements, 0, 1, 3, 1, 2, 3)
			 - matrix.elements[12] * Matrix4Minor(matrix.elements, 0, 1, 2, 1, 2, 3);
	}

	Matrix4 adjugate(const Matrix4& matrix)
	{
		// Github Copilot
		//return Matrix4(
		//	 Matrix4Minor(matrix.elements, 1, 2, 3, 1, 2, 3), -Matrix4Minor(matrix.elements, 0, 2, 3, 1, 2, 3),  Matrix4Minor(matrix.elements, 0, 1, 3, 1, 2, 3), -Matrix4Minor(matrix.elements, 0, 1, 2, 1, 2, 3),
		//	-Matrix4Minor(matrix.elements, 1, 2, 3, 0, 2, 3),  Matrix4Minor(matrix.elements, 0, 2, 3, 0, 2, 3), -Matrix4Minor(matrix.elements, 0, 1, 3, 0, 2, 3),  Matrix4Minor(matrix.elements, 0, 1, 2, 0, 2, 3),
		//	 Matrix4Minor(matrix.elements, 1, 2, 3, 0, 1, 3), -Matrix4Minor(matrix.elements, 0, 2, 3, 0, 1, 3),  Matrix4Minor(matrix.elements, 0, 1, 3, 0, 1, 3), -Matrix4Minor(matrix.elements, 0, 1, 2, 0, 1, 3),
		//	-Matrix4Minor(matrix.elements, 1, 2, 3, 0, 1, 2),  Matrix4Minor(matrix.elements, 0, 2, 3, 0, 1, 2), -Matrix4Minor(matrix.elements, 0, 1, 3, 0, 1, 2),  Matrix4Minor(matrix.elements, 0, 1, 2, 0, 1, 2)
		//);
		// cofactor (M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
		Matrix4 cofactor;

		cofactor.elements[0] =   Matrix4Minor(matrix.elements, 1, 2, 3, 1, 2, 3);
		cofactor.elements[1] =  -Matrix4Minor(matrix.elements, 1, 2, 3, 0, 2, 3);
		cofactor.elements[2] =   Matrix4Minor(matrix.elements, 1, 2, 3, 0, 1, 3);
		cofactor.elements[3] =  -Matrix4Minor(matrix.elements, 1, 2, 3, 0, 1, 2);
		cofactor.elements[4] =  -Matrix4Minor(matrix.elements, 0, 2, 3, 1, 2, 3);
		cofactor.elements[5] =   Matrix4Minor(matrix.elements, 0, 2, 3, 0, 2, 3);
		cofactor.elements[6] =  -Matrix4Minor(matrix.elements, 0, 2, 3, 0, 1, 3);
		cofactor.elements[7] =   Matrix4Minor(matrix.elements, 0, 2, 3, 0, 1, 2);
		cofactor.elements[8] =   Matrix4Minor(matrix.elements, 0, 1, 3, 1, 2, 3);
		cofactor.elements[9] =  -Matrix4Minor(matrix.elements, 0, 1, 3, 0, 2, 3);
		cofactor.elements[10] =  Matrix4Minor(matrix.elements, 0, 1, 3, 0, 1, 3);
		cofactor.elements[11] = -Matrix4Minor(matrix.elements, 0, 1, 3, 0, 1, 2);
		cofactor.elements[12] = -Matrix4Minor(matrix.elements, 0, 1, 2, 1, 2, 3);
		cofactor.elements[13] =  Matrix4Minor(matrix.elements, 0, 1, 2, 0, 2, 3);
		cofactor.elements[14] = -Matrix4Minor(matrix.elements, 0, 1, 2, 0, 1, 3);
		cofactor.elements[15] =  Matrix4Minor(matrix.elements, 0, 1, 2, 0, 1, 2);

		return transposed(cofactor);
	}

	Matrix4 inverse(const Matrix4& matrix)
	{
		float det = determinant(matrix);

		if (det == 0)
		{
			return Matrix4();
		}

		return adjugate(matrix) / det;
	}

	void invert(Matrix4& matrix)
	{
		matrix = inverse(matrix);
	}

	Matrix4 frustum(float left, float right, float bottom, float top, float znear, float zfar)
	{
		if (left == right || top == bottom || znear == zfar) {
			std::cout << "Invalid frustum\n";
			return Matrix4(); // Error
		}

		return Matrix4(
			(2.0f * znear) / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, (2.0f * znear) / (top - bottom), 0.0f, 0.0f,
			(right + left) / (right - left), (top + bottom) / (top - bottom), (-(zfar + znear)) / (zfar - znear), -1.0f,
			0.0f, 0.0f, (-2.0f * zfar * znear) / (zfar - znear), 0.0f
		);
	}

	Matrix4 perspective(float fov, float aspect, float znear, float zfar)
	{
		float ymax = znear * tanf(fov * PI / DEGREE_360);
		float xmax = ymax * aspect;
		return frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
	}

	Matrix4 ortho(float left, float right, float bottom, float top, float znear, float zfar)
	{
		if (left == right || top == bottom || znear == zfar) {
			return Matrix4(); // Error
		}
		
		return Matrix4(
			2.0f / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / (zfar - znear), 0.0f,
			-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((zfar + znear) / (zfar - znear)), 1.0f
		);
	}
	
	Matrix4 lookAt(const Vector3& position, const Vector3& target, const Vector3& worldUp)
	{
		Vector3 forward = normalized(target - position) * -1.0f;
		Vector3 right = cross(worldUp, forward); // Right handed
		if (right == Vector3(0.0f, 0.0f, 0.0f)) {
			return Matrix4(); // Error
		}
		
		normalize(right);
		Vector3 realUp = normalized(cross(forward, right)); // Right handed
		Vector3 translate = Vector3(
			-dot(right, position),
			-dot(realUp, position),
			-dot(forward, position)
		);
		
		return Matrix4(
			// Transpose upper 3x3 matrix to invert it
			right.x, realUp.x, forward.x, 0.0f,
			right.y, realUp.y, forward.y, 0.0f,
			right.z, realUp.z, forward.z, 0.0f,
			translate.x, translate.y, translate.z, 1.0f
		);
	}
}