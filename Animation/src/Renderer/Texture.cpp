#include "Texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Loader/stb_image.h>

namespace Renderer
{
	Texture::Texture()
	{
		width = 0;
		height = 0;
		channels = 0;
		glGenTextures(1, &handle);
	}

	Texture::Texture(const std::string& path)
	{
		glGenTextures(1, &handle);
		load(path);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &handle);
	}

	void Texture::load(const std::string& path)
	{
		glBindTexture(GL_TEXTURE_2D, handle);
		
		uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		stbi_image_free(data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::bind(uint32_t uniformIndex, uint32_t textureIndex)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, handle);
		glUniform1i(uniformIndex, textureIndex);
	}
	
	void Texture::unbind(uint32_t textureIndex)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
	}
}