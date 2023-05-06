#include "RearrangeBones.h"

#include <list>

namespace Animation
{
	BoneMap rearrangeSkeleton(Skeleton& skeleton)
	{
		const AnimationPose& restPose = skeleton.getRestPose();
		const AnimationPose& bindPose = skeleton.getBindPose();

		uint32_t size = restPose.getSize();

		if (size == 0)
		{
			return BoneMap();
		}

		std::vector<std::vector<int32_t>> hierarcchy(size);
		std::list<int32_t> process;
		
		for (uint32_t i = 0; i < size; i++)
		{
			// If a joint has a parent, add the index of the joint to the parent's vector of children 
			// nodes.If a node is a root node(so it has no parent), add it directly to the process
			// list.This list will be used later to traverse the map depth :
			int32_t parent = restPose.getParent(i);

			if (parent >= 0)
			{
				hierarcchy[parent].push_back(i);
			}
			else
			{
				process.push_back(i);
			}
		}

		BoneMap mapForward;
		BoneMap mapBackward;

		// For each element, if it contains children, add the children to the process list.This
		// way, all the joints are processed and the joints higher up in the transform hierarchy
		// are processed first
		int32_t index = 0;

		while (process.size() > 0)
		{
			int32_t current = *process.begin();
			process.pop_front();
			std::vector<int32_t>& children = hierarcchy[current];

			uint32_t numChildren = static_cast<uint32_t>(children.size());
			
			for (uint32_t i = 0; i < numChildren; i++)
			{
				process.push_back(children[i]);
			}

			mapForward[index] = current;
			mapBackward[current] = index;
			index += 1;
		}
		
		mapBackward[-1] = -1;
		mapBackward[-1] = -1;

		// Now that the maps are filled in, you need to build new rest and bind poses whose
		// bones are in the correct order.Loop through every joint in the original rest and
		// bind poses and copy their local transforms to the new poses.Do the same thing for
		// the joint names
		AnimationPose newRestPose(size);
		AnimationPose newBindPose(size);
		std::vector<std::string> newNames(size);

		for (uint32_t i = 0; i < size; i++)
		{
			int32_t currentBone = mapForward[i];
			
			newRestPose.setLocalTransform(i, restPose.getLocalTransform(currentBone));
			newBindPose.setLocalTransform(i, bindPose.getLocalTransform(currentBone));
			newNames[i] = skeleton.getJointName(currentBone);

			// Finding the new parent joint ID for each joint requires two mapping steps.First,
			// map the current index to the bone in the original skeleton.This returns the parent
			// of the original skeleton.Map this parent index back to the new skeleton.This is why
			// there are two dictionaries, to make this mapping fast
			int32_t parent = mapBackward[bindPose.getParent(currentBone)];
			
			newRestPose.setParent(i, parent);
			newBindPose.setParent(i, parent);
		}

		skeleton.set(newRestPose, newBindPose, newNames);

		return mapBackward;
	}

	void rearrangeAnimationClip(AnimationClip& animationClip, BoneMap& boneMap)
	{
		// To rearrange an animation clip, loop through all the tracks in the clip.For each track, find
		// the joint ID, then convert that joint ID using the(backward) bone map that was returned
		// by the RearrangeSkeleton function.Write the modified joint ID back into the tack
		uint32_t size = animationClip.getSize();

		for (uint32_t i = 0; i < size; i++)
		{
			int32_t jointId = animationClip.getJointIdAtIndex(i);
			uint32_t newJointId = static_cast<uint32_t>(boneMap[jointId]);
			animationClip.setJointIdAtIndex(i, newJointId);
		}
	}
	
	void rearrangeFastAnimationClip(AnimationClip& animationClip, BoneMap& boneMap)
	{
		// To rearrange an animation clip, loop through all the tracks in the clip.For each track, find
		// the joint ID, then convert that joint ID using the(backward) bone map that was returned
		// by the RearrangeSkeleton function.Write the modified joint ID back into the tack
		uint32_t size = animationClip.getSize();

		for (uint32_t i = 0; i < size; i++)
		{
			int32_t jointId = animationClip.getJointIdAtIndex(i);
			uint32_t newJointId = static_cast<uint32_t>(boneMap[jointId]);
			animationClip.setJointIdAtIndex(i, newJointId);
		}
	}

	void rearrangeSkeletalMesh(SkeletalMesh& skeletalMesh, BoneMap& boneMap)
	{
		std::vector<Vector4i>& influencesJoints = skeletalMesh.getInfluenceJoints();
		uint32_t size = static_cast<uint32_t>(influencesJoints.size());

		for (uint32_t i = 0; i < size; i++)
		{
			influencesJoints[i].x = boneMap[influencesJoints[i].x];
			influencesJoints[i].y = boneMap[influencesJoints[i].y];
			influencesJoints[i].z = boneMap[influencesJoints[i].z];
			influencesJoints[i].w = boneMap[influencesJoints[i].w];
		}

		skeletalMesh.updateOpenGLBuffers();
	}
}