#include "Skeleton.h"

namespace Animation
{
	
	Skeleton::Skeleton()
	{

	}

	Skeleton::Skeleton(const AnimationPose& inRestPose, const AnimationPose& inBindPose, const std::vector<std::string>& inJointNames)
	{
		set(inRestPose, inBindPose, inJointNames);
	}

	void Skeleton::set(const AnimationPose& inRestPose, const AnimationPose& inBindPose, const std::vector<std::string>& inJointNames)
	{
		restPose = inRestPose;
		bindPose = inBindPose;
		jointNames = inJointNames;
		updateInverseBindPose();
	}

	const AnimationPose& Skeleton::getBindPose() const
	{
		return bindPose;
	}

	const AnimationPose& Skeleton::getRestPose() const
	{
		return restPose;
	}

	const std::vector<Matrix4>& Skeleton::getInverseBindPose() const
	{
		return inverseBindPose;
	}

	void Skeleton::getInverseBindPose(std::vector<DualQuaternion>& outInverseBindPose)
	{
		uint32_t size = bindPose.getSize();

		outInverseBindPose.resize(size);

		for (uint32_t i = 0; i < size; i++)
		{
			DualQuaternion world = bindPose.getGlobalDualQuaternion(i);
			outInverseBindPose[i] = conjugate(world);
		}
	}

	const std::vector<std::string>& Skeleton::getJointNames() const
	{
		return jointNames;
	}

	const std::string& Skeleton::getJointName(uint32_t index) const
	{
		return jointNames[index];
	}

	void Skeleton::updateInverseBindPose()
	{
		uint32_t size = bindPose.getSize();
		inverseBindPose.resize(size);

		for (uint32_t i = 0; i < size; i++)
		{
			Transform world = bindPose.getGlobalTransform(i);
			inverseBindPose[i] = inverse(transformToMatrix4(world));
		}
	}
}