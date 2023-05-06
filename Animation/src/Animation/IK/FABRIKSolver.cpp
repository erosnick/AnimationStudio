#include "FABRIKSolver.h"

namespace Animation
{
	FABRIKSolver::FABRIKSolver()
	{
		steps = 15;
		threshold = 0.00001f;
	}

	uint32_t FABRIKSolver::getSize() const
	{
		return static_cast<uint32_t>(IKChain.size());
	}

	void FABRIKSolver::resize(uint32_t newSize)
	{
		IKChain.resize(newSize);
		worldChain.resize(newSize);
		lengths.resize(newSize);
	}

	uint32_t FABRIKSolver::getSteps()
	{
		return steps;
	}

	void FABRIKSolver::setSteps(uint32_t newSteps)
	{
		steps = newSteps;
	}

	float FABRIKSolver::getThreshold() const
	{
		return threshold;
	}

	void FABRIKSolver::setThreshold(float value)
	{
		threshold = value;
	}

	Transform FABRIKSolver::getLocalTransform(uint32_t index) const
	{
		return IKChain[index];
	}

	void FABRIKSolver::setLocalTransform(uint32_t index, const Transform& transform)
	{
		IKChain[index] = transform;
	}

	Transform FABRIKSolver::getGlobalTransform(uint32_t index)
	{
		uint32_t size = static_cast<uint32_t>(IKChain.size());

		Transform world = IKChain[index];

		for (int32_t i = index - 1; i >= 0; --i)
		{
			world = combine(IKChain[i], world);
		}

		return world;
	}

	bool FABRIKSolver::solve(const Transform& target)
	{
		uint32_t size = getSize();

		if (size == 0)
		{
			return false;
		}

		uint32_t last = size - 1;

		float thresholdSqured = threshold * threshold;

		IKChainToWorld();

		Vector3 goal = target.position;
		Vector3 base = worldChain[0];

		// Iterate from 0 to steps.For each iteration, check whether the goal and
		// end effector are close enough for the chain to be solved.If they are, copy the
		// world positions back into the chain with the worldToIKChain helper function
		// and return early.If they are not close enough, do the iteration by calling the
		// iterateBackward and iterateForward methods
		for (uint32_t i = 0; i < steps; i++)
		{
			Vector3 effector = worldChain[last];

			if (lengthSqured(goal - effector) < thresholdSqured)
			{
				worldToIKChain();
				return true;
			}

			iterateBackward(goal);
			iterateForward(base);
		}

		// After the iteration loop, copy the world positions vector back into the IK chain
		// regardless of whether the solver was able to solve the chain.Check one last time
		// whether the end effector has reached its goal, and return the appropriate Boolean
		worldToIKChain();

		Vector3 effector = getGlobalTransform(last).position;
		
		if (lengthSqured(goal - effector) < thresholdSqured)
		{
			return true;
		}

		return false;
	}

	void FABRIKSolver::IKChainToWorld()
	{
		// Implement the IKChainToWorld function, which will copy the IK chain into the
		// world transform vector and record the segment lengths.The lengths array stores the
		// distance of a joint from its parent.This means that the root joint will always contain
		// the length 0. For non - root joints, the distance at the i index is the distance between
		// the joints i and i ¨C 1 :
		uint32_t size = getSize();
		
		for (uint32_t i = 0; i < size; i++)
		{
			Transform world = getGlobalTransform(i);
			worldChain[i] = world.position;

			if (i >= 1)
			{
				Vector3 previous = worldChain[i - 1];
				lengths[i] = length(world.position - previous);
			}
		}

		// Root joint will always contain the length 0 
		if (size > 0)
		{
			lengths[0] = 0.0f;
		}
	}

	void FABRIKSolver::iterateBackward(const Vector3& base)
	{
		// Next, implement the IterateBackward function, which sets the last element in
		// the chain to be at the goal.This breaks the IK chain.Adjust all other joints using the
		// stored distanced so that the chain remains intact.After this function executes, the
		// end effector is always at the goal and the initial joint is probably no longer at
		// the base :
		int32_t size = getSize();
		
		if (size > 0)
		{
			worldChain[size - 1] = base;
		}

		for (int32_t i = size - 2; i >= 0; --i)
		{
			Vector3 direction = normalized(worldChain[i] - worldChain[i + 1]);
			Vector3 offset = direction * lengths[i + 1];
			worldChain[i] = worldChain[i + 1] + offset;
		}
	}

	void FABRIKSolver::iterateForward(const Vector3& goal)
	{
		// Next, implement the IterateBackward function, which sets the last element in
		// the chain to be at the goal.This breaks the IK chain.Adjust all other joints using the
		// stored distanced so that the chain remains intact.After this function executes, the
		// end effector is always at the goal and the initial joint is probably no longer at
		// the base
		int32_t size = getSize();

		if (size > 0)
		{
			worldChain[size - 1] = goal;
		}

		for (int32_t i = 1; i < size; i++)
		{
			Vector3 direction = normalized(worldChain[i] - worldChain[i - 1]);

			Vector3 offset = direction * lengths[i];
			worldChain[i] = worldChain[i - 1] + offset;
		}
	}

	void FABRIKSolver::worldToIKChain()
	{
		uint32_t size = getSteps();

		if (size == 0)
		{
			return;
		}

		for (uint32_t i = 0; i < size - 1; i++)
		{
			Transform world = getGlobalTransform(i);
			Transform next = getGlobalTransform(i + 1);

			Vector3 position = world.position;
			Quaternion rotation = world.rotation;

			// Create a vector that points from the current joint to the next one.This is the
			// rotation between the current node and the next node
			Vector3 toNext = next.position - position;
			toNext = inverse(rotation) * toNext;

			// Construct a vector that points from the world space IK chain of the next joint to
			// the current position.This is the rotation between the current node and the desired
			// position of the next node
			Vector3 toDesired = worldChain[i + 1] - position;
			toDesired = inverse(rotation) * toDesired;

			// Align these two vectors by using the fromTo quaternion function.Apply the final
			// delta rotation to the current joint's IK chain rotation
			Quaternion delta = fromTo(toNext, toDesired);
			IKChain[i].rotation = delta * IKChain[i].rotation;
		}
	}
}