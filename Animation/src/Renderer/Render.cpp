#include "Render.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Renderer
{
	static GLenum renderModeToGLEnum(RenderMode renderMode)
	{
		switch (renderMode)
		{
			case RenderMode::Points:
				return GL_POINTS;
			case RenderMode::Lines:
				return GL_LINES;
			case RenderMode::LineStrip:
				return GL_LINE_STRIP;
			case RenderMode::LineLoop:
				return GL_LINE_LOOP;
			case RenderMode::Triangles:
				return GL_TRIANGLES;
			case RenderMode::TriangleStrip:
				return GL_TRIANGLE_STRIP;
			case RenderMode::TriangleFan:
				return GL_TRIANGLE_FAN;
		}
		
		return 0;
	}

	void render(uint32_t vertexCount, RenderMode renderMode)
	{
		glDrawArrays(renderModeToGLEnum(renderMode), 0, vertexCount);
	}

	void render(const IndexBuffer& indexBuffer, RenderMode renderMode)
	{
		uint32_t handle = indexBuffer.getHandle();
		uint32_t numIndices = indexBuffer.getCount();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		glDrawElements(renderModeToGLEnum(renderMode), numIndices, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void renderInstanced(uint32_t vertexCount, RenderMode renderMode, uint32_t numInstances)
	{
		glDrawArraysInstanced(renderModeToGLEnum(renderMode), 0, vertexCount, numInstances);
	}

	void renderInstanced(const IndexBuffer& indexBuffer, RenderMode renderMode, uint32_t instanceCount)
	{
		uint32_t handle = indexBuffer.getHandle();
		uint32_t numIndices = indexBuffer.getCount();
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		glDrawElementsInstanced(renderModeToGLEnum(renderMode), numIndices, GL_UNSIGNED_INT, 0, instanceCount);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}