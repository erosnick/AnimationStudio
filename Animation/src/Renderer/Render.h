#pragma once

#include "IndexBuffer.h"

namespace Renderer
{
	enum class RenderMode
	{
		Points,
		LineStrip,
		LineLoop,
		Lines,
		Triangles,
		TriangleStrip,
		TriangleFan
	};

	void render(const IndexBuffer& indexBuffer, RenderMode renderMode);
	void render(uint32_t vertexCount, RenderMode renderMode);
	void renderInstanced(uint32_t vertexCount, RenderMode renderMode, uint32_t numInstances);
	void renderInstanced(const IndexBuffer& indexBuffer, RenderMode renderMode, uint32_t instanceCount);
}