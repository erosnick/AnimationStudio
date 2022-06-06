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

	void draw(const IndexBuffer& indexBuffer, RenderMode renderMode);
	void draw(uint32_t vertexCount, RenderMode renderMode);
	void drawInstanced(uint32_t vertexCount, RenderMode renderMode, uint32_t numInstances);
	void drawInstanced(const IndexBuffer& indexBuffer, RenderMode renderMode, uint32_t instanceCount);
}