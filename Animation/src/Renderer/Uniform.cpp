#include "Uniform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Math/Quaternion.h>
#include <Math/Matrix4.h>

namespace Renderer
{
#define UNIFORM_IMPLEMENT(glFunction, structType, dataType) \
	template <> void Uniform<structType>::set(uint32_t slot, const structType* data, uint32_t length) \
	{ \
		glFunction(slot, (GLsizei)length, (dataType*)&data[0]); \
	}
	
	template Uniform<int>;
	template Uniform<Math::Vector2i>;
	template Uniform<Math::Vector4i>;
	template Uniform<float>;
	template Uniform<Math::Vector2>;
	template Uniform<Math::Vector3>;
	template Uniform<Math::Vector4>;
	template Uniform<Math::Quaternion>;
	template Uniform<Math::Matrix4>;

	UNIFORM_IMPLEMENT(glUniform1iv, int, int)
	UNIFORM_IMPLEMENT(glUniform2iv, Math::Vector2i, int)		
	UNIFORM_IMPLEMENT(glUniform4iv, Math::Vector4i, int)
	UNIFORM_IMPLEMENT(glUniform1fv, float, float)
	UNIFORM_IMPLEMENT(glUniform2fv, Math::Vector2, float)
	UNIFORM_IMPLEMENT(glUniform3fv, Math::Vector3, float)
	UNIFORM_IMPLEMENT(glUniform4fv, Math::Vector4, float)
	UNIFORM_IMPLEMENT(glUniform4fv, Math::Quaternion, float)

	template <> void Uniform<Math::Matrix4>::set(uint32_t slot, const Math::Matrix4* data, uint32_t length)
	{
		glUniformMatrix4fv(slot, (GLsizei)length, false, (float*)&data[0]);
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