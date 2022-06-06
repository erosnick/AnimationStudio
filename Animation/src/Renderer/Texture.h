#pragma once

#include <cstdint>
#include <string>

namespace Renderer
{
	class Texture
	{
	public:
		Texture();
		Texture(const std::string& path);
		~Texture();

		void load(const std::string& path);
		void bind(uint32_t uniformIndex, uint32_t textureIndex);
		void unbind(uint32_t textureIndex);

		uint32_t getHandle() const { return handle; }
		
		Texture(const Texture& texture) = delete;
		Texture& operator=(const Texture& texture) = delete;

	private:
		int32_t width;
		int32_t height;
		int32_t channels;
		uint32_t handle;
	};
}