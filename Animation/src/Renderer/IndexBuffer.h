#pragma once

#include <cstdint>
#include <vector>

namespace Renderer
{
	class IndexBuffer
	{
	public:
		IndexBuffer();
		~IndexBuffer();

		IndexBuffer(const IndexBuffer& indexBuffer) = delete;
		IndexBuffer& operator=(const IndexBuffer& indexBuffer) = delete;

		void set(const uint32_t* data, uint32_t length);
		void set(const std::vector<uint32_t>& data);
		
		uint32_t getCount() const { return count; }
		uint32_t getHandle() const { return handle; }
	private:
		uint32_t handle;
		uint32_t count;
	};
}