#include "Attribute.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

namespace Renderer
{
	template Attribute<int>;
	template Attribute<float>;
	template Attribute<Math::Vector2>;
	template Attribute<Math::Vector3>;
	template Attribute<Math::Vector4>;
	template Attribute<Math::Vector4i>;

	template <typename T>
	Attribute<T>::Attribute()
	{
		glGenBuffers(1, &handle);
		count = 0;
	}

	template <typename T>
	Attribute<T>::~Attribute()
	{
		glDeleteBuffers(1, &handle);
	}

	template <typename T>
	void Attribute<T>::set(const T* inputArray, uint32_t arrayLength)
	{
		count = arrayLength;
		uint32_t size = sizeof(T);

		glBindBuffer(GL_ARRAY_BUFFER, handle);
		glBufferData(GL_ARRAY_BUFFER, size * count, inputArray, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	template <typename T>
	void Attribute<T>::set(const std::vector<T>& inputArray)
	{
		set(&inputArray[0], (uint32_t)inputArray.size());
	}

	template <>
	void Attribute<int>::setAttributePointer(uint32_t slot)
	{
		glVertexAttribIPointer(slot, 1, GL_INT, 0, (void*)0);
	}

	template <>
	void Attribute<Math::Vector4i>::setAttributePointer(uint32_t slot)
	{
		glVertexAttribIPointer(slot, 4, GL_INT, 0, (void*)0);
	}

	template <>
	void Attribute<float>::setAttributePointer(uint32_t slot)
	{
		glVertexAttribPointer(slot, 1, GL_FLOAT, false, 0, 0);
	}

	template <>
	void Attribute<Math::Vector2>::setAttributePointer(uint32_t slot)
	{
		glVertexAttribPointer(slot, 2, GL_FLOAT, false, 0, 0);
	}

	template <>
	void Attribute<Math::Vector3>::setAttributePointer(uint32_t slot)
	{
		glVertexAttribPointer(slot, 3, GL_FLOAT, false, 0, 0);
	}

	template <>
	void Attribute<Math::Vector4>::setAttributePointer(uint32_t slot)
	{
		glVertexAttribPointer(slot, 4, GL_FLOAT, false, 0, 0);
	}

	template <typename T>
	void Attribute<T>::bindTo(uint32_t slot)
	{
		glBindBuffer(GL_ARRAY_BUFFER, handle);
		glEnableVertexAttribArray(slot);
		setAttributePointer(slot);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template <typename T>
	void Attribute<T>::unbindFrom(uint32_t slot)
	{
		glBindBuffer(GL_ARRAY_BUFFER, handle);
		glDisableVertexAttribArray(slot);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}