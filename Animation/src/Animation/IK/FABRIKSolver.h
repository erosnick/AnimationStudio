#pragma once

#include <Math/Vector3.h>
#include <Math/Transform.h>

#include <vector>
#include <cstdint>

using namespace Math;

namespace Animation
{
	class FABRIKSolver
	{
	public:
		FABRIKSolver();

		uint32_t getSize() const;
		void resize(uint32_t newSize);

		uint32_t getSteps();
		void setSteps(uint32_t newSteps);

		float getThreshold() const;
		void setThreshold(float value);

		Transform getLocalTransform(uint32_t index) const;
		void setLocalTransform(uint32_t index, const Transform& transform);
		Transform getGlobalTransform(uint32_t index);

		bool solve(const Transform& target);

	protected:
		void IKChainToWorld();
		void iterateBackward(const Vector3& base);
		void iterateForward(const Vector3& goal);
		void worldToIKChain();
		
	protected:
		std::vector<Transform> IKChain;
		uint32_t steps;
		float threshold;
		std::vector<Vector3> worldChain;
		std::vector<float> lengths;
	};
}