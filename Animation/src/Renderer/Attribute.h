#pragma once

#include <cstdint>
#include <vector>

namespace Renderer
{
	template <typename T>
	class Attribute
	{
	public:
		Attribute();
		~Attribute();

		Attribute(const Attribute& attribute) = delete;
		Attribute& operator=(const Attribute& attribute) = delete;

		void set(const T* inputArray, uint32_t arrayLength);
		void set(const std::vector<T>& input);

		void bindTo(uint32_t slot);
		void unbindFrom(uint32_t slot);

		uint32_t getCount() const { return count; }
		uint32_t getHandle() const { return handle; }

	private:
		void setAttributePointer(uint32_t slot);

	protected:
		uint32_t handle;
		uint32_t count;
	};
}