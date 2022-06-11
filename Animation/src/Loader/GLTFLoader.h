#pragma once

#include <Loader/cgltf.h>
#include <string>
#include <vector>

#include <Animation/SkeletalMesh.h>
#include <Animation/Skeleton.h>
#include <Animation/AnimationClip.h>

using namespace Animation;

namespace Loader
{
	cgltf_data* loadGLTFFile(const std::string& path);
	void freeGLTFFile(cgltf_data* data);
	AnimationPose loadRestPose(cgltf_data* data);
	std::vector<std::string> loadJointNames(cgltf_data* data);
	std::vector<AnimationClip> loadAnimationClips(cgltf_data* data);
	AnimationPose loadBindPose(cgltf_data* data);
	Skeleton loadSkeleton(cgltf_data* data);
	std::vector<SkeletalMesh> loadMeshes(cgltf_data* data);
}