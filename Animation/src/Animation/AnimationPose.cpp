#include "AnimationPose.h"

namespace Animation
{
	AnimationPose::AnimationPose()
	{
	}

	AnimationPose::AnimationPose(const AnimationPose& other)
	{
		*this = other;
	}

	AnimationPose::AnimationPose(int32_t numJoints)
	{
		resize(numJoints);
	}

	AnimationPose::~AnimationPose()
	{
	}

	AnimationPose& AnimationPose::operator=(const AnimationPose& other)
	{
		if (&other == this)
		{
			return *this;
		}

		if (parents.size() != other.parents.size())
		{
			parents.resize(other.parents.size());
		}

		if (joints.size() != other.joints.size())
		{
			joints.resize(other.joints.size());
		}

		if (parents.size() != 0)
		{
			memcpy_s(&parents[0], parents.size() * sizeof(int32_t), &other.parents[0], parents.size() * sizeof(int32_t));
		}

		if (joints.size() != 0)
		{
			memcpy_s(&joints[0], joints.size() * sizeof(Transform), &other.joints[0], joints.size() * sizeof(Transform));
		}

		return *this;
	}

	void AnimationPose::resize(uint32_t newSize)
	{
		parents.resize(newSize);
		joints.resize(newSize);
	}

	uint32_t AnimationPose::getSize() const
	{
		return static_cast<uint32_t>(joints.size());
	}

	int32_t AnimationPose::getParent(uint32_t index) const
	{
		return parents[index];
	}

	void AnimationPose::setParent(uint32_t index, int parent)
	{
		parents[index] = parent;
	}

	const Transform& AnimationPose::getLocalTransform(uint32_t index) const
	{
		return joints[index];
	}

	void AnimationPose::setLocalTransform(uint32_t index, const Transform& transform)
	{
		joints[index] = transform;
	}

	Transform AnimationPose::getGlobalTransform(uint32_t index) const
	{
		Transform result = joints[index];

		for (int32_t parentId = parents[index]; parentId >= 0; parentId = parents[parentId])
		{
			result = combine(joints[parentId], result);
		}

		return result;
	}

	const Transform AnimationPose::operator[](uint32_t index) const
	{
		return getGlobalTransform(index);
	}

	void AnimationPose::getMatrixPalette(std::vector<Matrix4>& out) const
	{
		uint32_t size = getSize();

		if (out.size() != size)
		{
			out.resize(size);
		}

		for (uint32_t i = 0; i < size; i++)
		{
			Transform transform = getGlobalTransform(i);
			out[i] = transformToMatrix4(transform);
		}
	}

	bool AnimationPose::operator!=(const AnimationPose& other)
	{
		return !(*this == other);
	}

	bool AnimationPose::operator==(const AnimationPose& other)
	{
		if (joints.size() != other.joints.size())
		{
			return false;
		}

		if (parents.size() != other.parents.size())
		{
			return false;
		}

		uint32_t size = static_cast<uint32_t>(joints.size());

		for (uint32_t i = 0; i < size; i++)
		{
			Transform local = joints[i];
			Transform otherLocal = other.joints[i];
			int32_t parent = parents[i];
			int32_t otherParent = other.parents[i];

			if (parent != otherParent)
			{
				return false;
			}

			if (local.position != otherLocal.position)
			{
				return false;
			}

			if (local.rotation != otherLocal.rotation)
			{
				return false;
			}

			if (local.scale != otherLocal.scale)
			{
				return false;
			}
		}

		return true;
	}
}

