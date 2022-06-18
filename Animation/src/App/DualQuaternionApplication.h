#pragma once

#include <memory>

#include "Application.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"

#include "Renderer/Shader.h"
#include "Renderer/Attribute.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/DebugDraw.h"

#include <Animation/SkeletalMesh.h>
#include "Animation/AnimationPose.h"
#include "Animation/AnimationClip.h"
#include <Animation/Skeleton.h>

using namespace Math;
using namespace Renderer;
using namespace Debug;

struct GLFWwindow;

struct AnimationInstance {
	AnimationPose animationPose;
	std::vector <Matrix4> animationPosePalette;
	unsigned int clipIndex;
	float time;
	Transform model;

	inline AnimationInstance() : clipIndex(0), time(0.0f) { }
};

class DualQuaternionApplication : public Application
{
public:
	void startup() override;

	void initGLFW();
	void initImGui();

	void prepareRenderResources();
	
	void prepareAnimationDebugData();

	void shutdown() override;
	void update(float deltaTime) override;

	void updateAnimationPose(float deltaTime);

	void run() override;
	void render() override;
	
	bool WGLExtensionSupported(const std::string& extension);

	void toggleVSync();
	
	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void onFramebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height);
	static void onMouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	static void onKeyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	// ---------------------------------------------------------------------------------------------------------
	void processInput();

private:
	void updateImGui();

protected:
	// settings
	const unsigned int SCREEN_WIDTH = 1280;
	const unsigned int SCREEN_HEIGHT = 720;
	GLFWwindow* window = nullptr;

	std::shared_ptr<Shader> linearBlendingSkinningShader;
	std::shared_ptr<Shader> dualQuaternionSkinningShader;
	std::shared_ptr<Texture> displayTexture;

	Vector3 eye = { 0.0f, 3.0f, 14.0f };
	Vector3 center = { 0.0f, 0.0f, 0.0f };

	std::vector<SkeletalMesh> meshes;
	Skeleton skeleton;

	AnimationPose currentPose;

	std::vector<DualQuaternion> dualQuaternionPosePalette;
	std::vector<DualQuaternion> dualQuaternionInverseBindPosePalette;
	std::vector<Matrix4> linearBlendingPosePalette;
	std::vector<Matrix4> linearBlendingInverseBindPosePalette;
		
	int32_t currentClip;
	std::vector<FastAnimationClip> fastAnimationClips;
	std::vector<std::string> animationNames;
	std::vector<char*> animationNamesArray;

	float playbackTime;
	int32_t currentFrame = 0;
	
	// Our state
	bool bShowDemoWindow = true;
	bool bShowAnotherWindow = false;
	bool bVSync = false;

	bool bPrecomputeSkin = true;

	uint32_t VAO = 0;
	uint32_t animationVAO = 0;

	Vector4 clearColor = { 0.45f, 0.55f, 0.60f, 1.0f };
private:
	void renderImGui();
};