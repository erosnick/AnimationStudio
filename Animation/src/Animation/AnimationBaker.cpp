#pragma once

#include "AnimationBaker.h"

namespace Animation
{
	void bakeAnimationToTexture(const Skeleton& skeleton, AnimationClip& animationClip, AnimationTexture& texture)
	{
		const AnimationPose& bindPose = skeleton.getBindPose();

		AnimationPose pose = bindPose;

		uint32_t textureWidth = texture.getSize();

		for (uint32_t x = 0; x < textureWidth; x++)
		{
			float t = static_cast<float>(x) / (textureWidth - 1);
			float start = animationClip.getStartTime();
			float time = start + animationClip.getDuration() * t;
			
			animationClip.sample(pose, time);

			for (uint32_t y = 0; y < pose.getSize() * 3; y += 3)
			{
				Transform node = pose.getGlobalTransform(y / 3);

				texture.setTexel(x, y + 0, node.position);
				texture.setTexel(x, y + 1, node.rotation);
				texture.setTexel(x, y + 2, node.scale);
			}
		}
		texture.uploadTextureDataToGPU();
	}
}