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

		// Slow path
#if 0
		for (uint32_t i = 0; i < size; i++)
		{
			Transform transform = getGlobalTransform(i);
			out[i] = transformToMatrix4(transform);
		}
		// Fast path
#else

		uint32_t i = 0;
		
		for (i = 0; i < size; i++)
		{
			int32_t parent = parents[i];

			if (parent > static_cast<int32_t>(i))
			{
				break;
			}

			Matrix4 global = transformToMatrix4(joints[i]);
			
			if (parent >= 0)
			{
				global = out[parent] * global;
			}

			out[i] = global;
		}

		uint32_t j = i;

		for (j = 0; j < size; j++)
		{
			Transform globalTransform = getGlobalTransform(j);
			out[j] = transformToMatrix4(globalTransform);
		}
#endif
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

	bool isInHierarchy(const AnimationPose& animationPose, uint32_t parent, uint32_t search)
	{
		// To check whether one joint is the descendant of another, follow the descendant joint
		// all the way up the hierarchy until the root node.If any of the nodes encountered in
		// this hierarchy are the node that you are checking against, return true
		if (search == parent)
		{
			return true;
		}

		int32_t currentParent = animationPose.getParent(search);

		while (currentParent >= 0)
		{
			if (currentParent == parent)
			{
				return true;
			}

			currentParent = animationPose.getParent(currentParent);
		}

		return false;
	}

	void blend(AnimationPose& result, const AnimationPose& a, const AnimationPose& b, float t, int32_t root)
	{
		// If two animations are blended using the whole hierarchy, the root argument to Blend
		// will be negative.With a negative joint for the blend root, the Blend function skips the
		// isInHierarchy check
		uint32_t numJoints = result.getSize();

		for (uint32_t i = 0; i < numJoints; i++)
		{
			if (root >= 0)
			{
				if (!isInHierarchy(result, root, i))
				{
					continue;
				}
			}

			result.setLocalTransform(i, lerp(a.getLocalTransform(i), b.getLocalTransform(i), t));
		}
	}
}

