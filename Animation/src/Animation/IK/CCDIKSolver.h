#pragma once

#include <Math/Transform.h>

#include <vector>
#include <cstdint>

using namespace Math;

namespace Animation
{
	namespace IK
	{
		class CCDIKSolver
		{
		public:
			CCDIKSolver();

			uint32_t getSize() const;
			void resize(uint32_t newSize);
			
			Transform& operator[](uint32_t index);
			Transform getGlobalTransform(uint32_t index);
			uint32_t getSteps() const;
			void setSteps(uint32_t newSteps);
			
			float getThreshold() const;
			void setThreshold(float value);

			bool solve(const Transform& target);
		protected:
			uint32_t steps;
			float threshold;
			std::vector<Transform> IKChain;
		};
	}
}