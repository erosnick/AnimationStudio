#include "Uniform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Math/Matrix4.h>
#include <Math/Quaternion.h>
#include <Math/DualQuaternion.h>

using namespace Math;

namespace Renderer
{
#define UNIFORM_IMPLEMENT(glFunction, structType, dataType) \
	template <> void Uniform<structType>::set(uint32_t slot, const structType* data, uint32_t length) \
	{ \
		glFunction(slot, (GLsizei)length, (dataType*)&data[0]); \
	}
	
	template Uniform<int>;
	template Uniform<Vector2i>;
	template Uniform<Vector4i>;
	template Uniform<float>;
	template Uniform<Vector2>;
	template Uniform<Vector3>;
	template Uniform<Vector4>;
	template Uniform<Quaternion>;
	template Uniform<Matrix4>;
	template Uniform<DualQuaternion>;

	UNIFORM_IMPLEMENT(glUniform1iv, int, int)
	UNIFORM_IMPLEMENT(glUniform2iv, Vector2i, int)		
	UNIFORM_IMPLEMENT(glUniform4iv, Vector4i, int)
	UNIFORM_IMPLEMENT(glUniform1fv, float, float)
	UNIFORM_IMPLEMENT(glUniform2fv, Vector2, float)
	UNIFORM_IMPLEMENT(glUniform3fv, Vector3, float)
	UNIFORM_IMPLEMENT(glUniform4fv, Vector4, float)
	UNIFORM_IMPLEMENT(glUniform4fv, Quaternion, float)

	template <> 
	void Uniform<Matrix4>::set(uint32_t slot, const Matrix4* data, uint32_t length)
	{
		glUniformMatrix4fv(slot, (GLsizei)length, false, (float*)&data[0]);
	}

	template <>
	void Uniform<DualQuaternion>::set(uint32_t slot, const DualQuaternion* data, uint32_t length)
	{
		glUniformMatrix2x4fv(slot, (GLsizei)length, false, data[0].data);
	}
		
	template <typename T>
	void Uniform<T>::set(uint32_t slot, const T& value)
	{
		set(slot, (T*)&value, 1);
	}

	template <typename T>
	void Uniform<T>::set(uint32_t slot, const std::vector<T>& data)
	{
		set(slot, &data[0], (uint32_t)data.size());
	}
}