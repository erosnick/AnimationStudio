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
	unsigned int clip;
	float playbackTime;
	Transform model;

	inline AnimationInstance() : clip(0), playbackTime(0.0f) { }
};

class DemoApplication : public Application
{
public:
	void startup() override;

	void initGLFW();
	void initImGui();

	void prepareRenderResources();

	void prepareCubeData();
	void prepareDebugData();
	void prepareAnimationDebugData();

	void shutdown() override;
	void update(float deltaTime) override;

	void updateAnimationPose(float deltaTime);

	void run() override;
	void render() override;
	
	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void onFramebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height);
	static void onMouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	static void onKeyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	// ---------------------------------------------------------------------------------------------------------
	void processInput();

	void toggleUpdateRotation();

	void updateCPUSkin();			// CPU蒙皮更新(变换顶点时计算AnimationPosePalette * inverseBindPose)
	void updatePrecomputedCPUSkin();	// 预计算CPU蒙皮更新(变换顶点前预计算AnimationPosePalette * inverseBindPose)
	void updateGPUSkin();			// GPU蒙皮矩阵调色板更新(顶点着色器计算AnimationPosePalette * inverseBindPose)
	void updatePrecomputedGPUSkin();	// 预计算GPU蒙皮矩阵调色板更新(传入顶点着色器前预计算AnimationPosePalette * inverseBindPose)

private:
	void updateImGui();

protected:
	// settings
	const unsigned int SCREEN_WIDTH = 1280;
	const unsigned int SCREEN_HEIGHT = 720;
	GLFWwindow* window = nullptr;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> meshShader;
	std::shared_ptr<Shader> skinnedMeshShader;
	std::shared_ptr<Attribute<Vector3>> vertexPositions;
	std::shared_ptr<Attribute<Vector3>> vertexNormals;
	std::shared_ptr<Attribute<Vector2>> vertexTexCoords;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<Texture> displayTexture;
	std::shared_ptr<DebugDraw> debugDraw;
	std::shared_ptr<DebugDraw> restPoseDebugDraw;
	std::shared_ptr<DebugDraw> currentPoseDebugDraw;
	float angle;
	Vector3 eye = { 0.0f, 5.0f, 7.0f };
	Vector3 center = { 0.0f, 3.0f, 0.0f };
	bool bUpdateRotation = false;

	std::vector<SkeletalMesh> CPUSkinnedMeshes;
	std::vector<SkeletalMesh> GPUSkinnedMeshes;
	Skeleton skeleton;
	int32_t currentClip;
	std::vector<FastAnimationClip> fastAnimationClips;
	std::vector<std::string> animationNames;
	std::vector<char*> animationNamesArray;

	float playbackTime;
	int32_t currentFrame = 0;
	AnimationInstance GPUAnimationInfo;
	AnimationInstance CPUAnimationInfo;

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	Vector4 clearColor = { 0.45f, 0.55f, 0.60f, 1.0f };
private:
	void renderImGui();
};