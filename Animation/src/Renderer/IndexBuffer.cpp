#include "IndexBuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Renderer
{
	IndexBuffer::IndexBuffer()
	{
		glGenBuffers(1, &handle);
		count = 0;
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &handle);
	}

	void IndexBuffer::set(const uint32_t* data, uint32_t length)
	{
		count = length;
		uint32_t size = sizeof(uint32_t);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * count, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void IndexBuffer::set(const std::vector<uint32_t>& data)
	{
		set(&data[0], (uint32_t)data.size());
	}
}