#pragma once

#include <map>
#include <cstdint>

#include <Animation/Skeleton.h>
#include <Animation/SkeletalMesh.h>
#include <Animation/AnimationClip.h>

using namespace Animation;

namespace Animation
{
	using BoneMap = std::map<int32_t, int32_t>;

	BoneMap rearrangeSkeleton(Skeleton& skeleton);
	void rearrangeAnimationClip(AnimationClip& animationClip, BoneMap& boneMap);
	void rearrangeFastAnimationClip(AnimationClip& animationClip, BoneMap& boneMap);
	void rearrangeSkeletalMesh(SkeletalMesh& skeletalMesh, BoneMap& boneMap);
}