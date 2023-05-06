#pragma once

#include "Skeleton.h"
#include "AnimationClip.h"
#include "AnimationPose.h"

namespace Animation
{
	bool isInHierarchy(const AnimationPose& animationPose, uint32_t parent, uint32_t search);

	void blend(AnimationPose& result, const AnimationPose& a, const AnimationPose& b, float t, int32_t blendRoot);
	
	// Samples the additive clip at time 0 into an output pose.This output
	// pose is the reference that is used to add two poses together.
	template <typename TAniamtionClip>
	AnimationPose makeAdditivePose(const Skeleton& skeleton, TAniamtionClip& animationClip);
	void add(AnimationPose& outPose, const AnimationPose& inPose, const AnimationPose& additivePose, 
	   const AnimationPose& additiveBasePose, int32_t blendRoot);
}