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

using namespace Math;
using namespace Renderer;

struct GLFWwindow;

class DemoApplication : public Application
{
public:
	void startup() override;

	void prepareCubeData();

	void shutdown() override;
	void update(float deltaTime) override;
	void run() override;
	void render() override;
	
	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	
	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	// ---------------------------------------------------------------------------------------------------------
	void processInput();

protected:
	// settings
	const unsigned int SCREEN_WIDTH = 800;
	const unsigned int SCREEN_HEIGHT = 600;
	GLFWwindow* window = nullptr;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Attribute<Vector3>> vertexPositions;
	std::shared_ptr<Attribute<Vector3>> vertexNormals;
	std::shared_ptr<Attribute<Vector2>> vertexTexCoords;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<Texture> displayTexture;
	std::shared_ptr<DebugDraw> debugDraw;
	float angle;
	Vector3 eye = { 0.0f, 1.0f, 3.0f };
	Vector3 center = { 0.0f, 0.0f, 0.0f };
};