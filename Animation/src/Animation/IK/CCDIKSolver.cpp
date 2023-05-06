#include "CCDIKSolver.h"

namespace Animation
{
	namespace IK
	{
		
		CCDIKSolver::CCDIKSolver()
		{
			steps = 15;
			threshold = 0.00001f;
		}

		uint32_t CCDIKSolver::getSize() const
		{
			return static_cast<uint32_t>(IKChain.size());
		}

		void CCDIKSolver::resize(uint32_t newSize)
		{
			IKChain.resize(newSize);
		}

		Transform& CCDIKSolver::operator[](uint32_t index)
		{
			return IKChain[index];
		}

		Transform CCDIKSolver::getGlobalTransform(uint32_t index)
		{
			uint32_t size = static_cast<uint32_t>(IKChain.size());

			Transform world = IKChain[index];

			for (int32_t i = index - 1; i >= 0; i--)
			{
				world = combine(IKChain[i], world);
			}

			return world;
		}

		uint32_t CCDIKSolver::getSteps() const
		{
			return steps;
		}

		void CCDIKSolver::setSteps(uint32_t newSteps)
		{
			steps = newSteps;
		}

		float CCDIKSolver::getThreshold() const
		{
			return threshold;
		}

		void CCDIKSolver::setThreshold(float value)
		{
			threshold = value;
		}

		bool CCDIKSolver::solve(const Transform& target)
		{
			uint32_t size = getSize();

			if (size == 0)
			{
				return false;
			}

			uint32_t last = size - 1;

			float thresholdSqured = threshold * threshold;
			Vector3 goal = target.position;

			// Loop from 0 to mNumSteps to perform the correct number of iterations.On every
			// iteration, get the position of the end effector and check whether it's close enough to 
			// the goal.If it is close enough, return early
			for (uint32_t i = 0; i < steps; i++)
			{
				Vector3 effector = getGlobalTransform(last).position;

				if (lengthSqured(goal - effector) < thresholdSqured)
				{
					return true;
				}

				// In each iteration, loop through the entire IK chain. 
				// Start the iteration at size - 2; since size - 1 is
				// the last element, rotating he last element has no
				// effect on any bones:
				for (int32_t j = size - 2; j >= 0; --j)
				{
					// For each joint in the IK chain, get the world transform of the joint.Find a vector
					// from the position of the joint to the position of the end effector.Find another vector
					// from the position of the current joint to the position of the goal :
					effector = getGlobalTransform(last).position;

					Transform world = getGlobalTransform(j);
					Vector3 position = world.position;
					Quaternion rotation = world.rotation;

					Vector3 toEffector = effector - position;
					Vector3 toGoal = goal - position;

					// Next, find a quaternion that rotates from the position to effector vector to the
					// position to goal vector.There is an edge case in which the vector pointing to the
					// effector or to the goal could be a zero vector
					Quaternion effectorToGoal;

					if (lengthSqured(toGoal) > 0.00001f)
					{
						effectorToGoal = fromTo(toEffector, toGoal);
					}

					// Use this vector to rotate the joint into the correct orientation in the world space.
					// Rotate the world space orientation of the joint by the inverse of the joint's previous 
					// world rotation to move the quaternion back into the joint space
					Quaternion worldRotation = rotation * effectorToGoal;
					Quaternion localRotation = worldRotation * inverse(rotation);
					
					IKChain[j].rotation = localRotation * IKChain[j].rotation;    

					effector = getGlobalTransform(last).position;

					if (lengthSqured(goal - effector) < thresholdSqured)
					{
						return true;
					}
				}
			}

			return false;
		}
	}
}