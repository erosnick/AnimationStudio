#pragma once

#include <cstdint>
#include <vector>

namespace Renderer
{
	template <typename T>
	class Uniform
	{
	public:
		Uniform() = delete;
		Uniform(const Uniform& uniform) = delete;
		Uniform operator=(const Uniform& uniform) = delete;
		~Uniform() = delete;

		static void set(uint32_t slot, const T& value);
		static void set(uint32_t slot, T* data, uint32_t length);
		static void set(uint32_t slot, std::vector<T>& data);
	};
}