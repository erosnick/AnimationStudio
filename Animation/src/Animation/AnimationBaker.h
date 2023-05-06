#pragma once

#include "Skeleton.h"
#include "AnimationClip.h"
#include "AnimationTexture.h"

namespace Animation
{
	void bakeAnimationToTexture(const Skeleton& skeleton, AnimationClip& animationClip, AnimationTexture& texture);
}