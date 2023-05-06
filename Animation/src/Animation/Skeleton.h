#pragma once

#include "AnimationPose.h"

#include <Math/Matrix4.h>
#include <Math/DualQuaternion.h>

#include <string>
#include <vector>

using namespace Math;

namespace Animation
{
	class Skeleton
	{
	public:
		Skeleton();
		Skeleton(const AnimationPose& inRestPose, const AnimationPose& inBindPose, const std::vector<std::string>& inJointNames);

		void set(const AnimationPose& inRestPose, const AnimationPose& inBindPose, const std::vector<std::string>& inJointNames);

		const AnimationPose& getBindPose() const;
		const AnimationPose& getRestPose() const;
		const std::vector<Matrix4>& getInverseBindPose() const;
		void getInverseBindPose(std::vector<DualQuaternion>& outInverseBindPose);
		const std::vector<std::string>& getJointNames() const;
		const std::string& getJointName(uint32_t index) const;
		
	protected:
		void updateInverseBindPose();
		
	protected:
		AnimationPose restPose;
		AnimationPose bindPose;
		std::vector<Matrix4> inverseBindPose;
		std::vector<std::string> jointNames;
	};
}