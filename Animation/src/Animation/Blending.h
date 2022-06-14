#pragma once

#include "Skeleton.h"
#include "AnimationClip.h"
#include "AnimationPose.h"

namespace Animation
{
	// Samples the additive clip at time 0 into an output pose.This output
	// pose is the reference that is used to add two poses together.
	AnimationPose makeAdditivePose(const Skeleton& skeleton, AnimationClip& animationClip);
	void add(AnimationPose& outPose, const AnimationPose& inPose, const AnimationPose& additivePose, 
	   const AnimationPose& additiveBasePose, int32_t blendRoot);
}