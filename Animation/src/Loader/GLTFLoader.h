#pragma once

#include <Loader/cgltf.h>
#include <string>
#include <vector>

#include <Animation/AnimationClip.h>

using namespace Animation;

namespace Loader
{
	cgltf_data* loadGLTFFile(const std::string& path);
	void freeGLTFFile(cgltf_data* data);
	AnimationPose loadRestPose(cgltf_data* data);
	std::vector<std::string> loadJointNames(cgltf_data* data);
	std::vector<AnimationClip> loadAnimationClips(cgltf_data* data);
}