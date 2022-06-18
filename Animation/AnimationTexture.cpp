#include "AnimationTexture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <fstream>

namespace Animation
{
	
	AnimationTexture::AnimationTexture()
	{
		size = 0;
		glGenTextures(1, &handle);
	}

	AnimationTexture::AnimationTexture(const AnimationTexture& other)
	{
		size = 0;
		glGenTextures(1, &handle);
		
		*this = other;
	}

	AnimationTexture::~AnimationTexture()
	{
		glDeleteTextures(1, &handle);
	}

	AnimationTexture& AnimationTexture::operator=(const AnimationTexture& other)
	{
		if (this == &other)
		{
			return *this;
		}

		size = other.size;
		data = other.data;

		return *this;
	}
	
	void AnimationTexture::save(const std::string& path)
	{
		std::ofstream file;
		
		file.open(path, std::ios::out | std::ios::binary);
		
		if (!file.is_open())
		{
			spdlog::error("Couldn't open {0}\n", path);
		}

		file << size;
		
		if (size != 0)
		{
			file.write((char*)&data[0], sizeof(float) * (size * size * 4));
		}
		
		file.close();
	}

	void AnimationTexture::load(const std::string& path)
	{
		std::ifstream file;

		file.open(path, std::ios::in | std::ios::binary);

		if (!file.is_open())
		{
			spdlog::error("Couldn't open {0}\n", path);
		}

		file >> size;
		
		data.resize(size);
		
		file.read((char*)&data[0], sizeof(float) * (size * size * 4));
		file.close();

		uploadTextureDataToGPU();
	}

	void AnimationTexture::uploadTextureDataToGPU()
	{
		glBindTexture(GL_TEXTURE_2D, handle);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, &data[0]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	uint32_t AnimationTexture::getSize()
	{
		return size;
	}

	void AnimationTexture::resize(uint32_t newSize)
	{
		size = newSize;
		data.resize(size * size * 4);
	}

	const std::vector<float>& AnimationTexture::getData() const
	{
		return data;
	}

	void AnimationTexture::setTexel(uint32_t x, uint32_t y, const Vector3& value)
	{
		uint32_t index = (y * size + x) * 4;

		data[index] = value.x;
		data[index + 1] = value.y;
		data[index + 2] = value.z;
		data[index + 3] = 0.0f;
	}

	void AnimationTexture::setTexel(uint32_t x, uint32_t y, const Quaternion& value)
	{
		uint32_t index = (y * size + x) * 4;

		data[index] = value.x;
		data[index + 1] = value.y;
		data[index + 2] = value.z;
		data[index + 3] = value.w;
	}

	Math::Vector4 AnimationTexture::getTexel(uint32_t x, uint32_t y)
	{
		uint32_t index = (y * size + x) * 4;

		return Vector4(data[index + 0], 
					   data[index + 1], 
					   data[index + 2], 
					   data[index + 3]);
	}

	void AnimationTexture::bind(uint32_t uniformIndex, uint32_t textureIndex)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, handle);
		glUniform1i(uniformIndex, textureIndex);
	}

	void AnimationTexture::unbind(uint32_t textureIndex)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
	}

	uint32_t AnimationTexture::getHandle() const
	{
		return handle;
	}
}