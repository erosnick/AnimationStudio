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

using namespace Math;
using namespace Renderer;
using namespace Debug;

struct GLFWwindow;

class DemoApplication : public Application
{
public:
	void startup() override;

	void prepareCubeData();
	void prepareDebugData();
	void prepareAnimationDebugData();

	void shutdown() override;
	void update(float deltaTime) override;
	void run() override;
	void render() override;
	
	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void framebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height);
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void keyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	// ---------------------------------------------------------------------------------------------------------
	void processInput();

	void toggleUpdateRotation();

protected:
	// settings
	const unsigned int SCREEN_WIDTH = 800;
	const unsigned int SCREEN_HEIGHT = 600;
	GLFWwindow* window = nullptr;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> meshShader;
	std::shared_ptr<Attribute<Vector3>> vertexPositions;
	std::shared_ptr<Attribute<Vector3>> vertexNormals;
	std::shared_ptr<Attribute<Vector2>> vertexTexCoords;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<Texture> displayTexture;
	std::shared_ptr<DebugDraw> debugDraw;
	std::shared_ptr<DebugDraw> restPoseDebugDraw;
	std::shared_ptr<DebugDraw> currentPoseDebugDraw;
	float angle;
	Vector3 eye = { 0.0f, 4.0f, 7.0f };
	Vector3 center = { 0.0f, 4.0f, 0.0f };
	bool bUpdateRotation = false;

	std::vector<SkeletalMesh> skeletalMeshs;
	AnimationPose restPose;
	AnimationPose currentPose;
	uint32_t currentClip;
	std::vector<AnimationClip> animationClips;

	float playbackTime;
	int32_t currentFrame = 0;
};