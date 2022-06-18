#pragma once

#include <Math/Vector3.h>
#include <Math/Quaternion.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace Math;

namespace Animation
{
	class AnimationTexture
	{
	public:
		AnimationTexture();
		AnimationTexture(const AnimationTexture& other);
		AnimationTexture& operator=(const AnimationTexture& other);
		~AnimationTexture();
		
		void save(const std::string& path);
		void load(const std::string& path);
		
		void uploadTextureDataToGPU();

		uint32_t getSize();
		void resize(uint32_t newSize);
		
		const std::vector<float>& getData() const;

		void setTexel(uint32_t x, uint32_t y, const Vector3& value);
		void setTexel(uint32_t x, uint32_t y, const Quaternion& value);
		
		Vector4 getTexel(uint32_t x, uint32_t y);
		void bind(uint32_t uniformIndex, uint32_t textureIndex);
		void unbind(uint32_t textureIndex);
		
		uint32_t getHandle() const;
	protected:
		std::vector<float> data;
		uint32_t size;
		uint32_t handle;
	};
}	
