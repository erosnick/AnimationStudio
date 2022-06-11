#define _CRT_SECURE_NO_WARNINGS

#include "DemoApplication.h"

#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Math.h"

#include "Renderer/Uniform.h"
#include "Renderer/Renderer.h"
#include "Loader/GLTFLoader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <spdlog/spdlog.h>

#include <iostream>
#include <chrono>

constexpr float fixTimestep = 0.016f;

using namespace Animation;

uint32_t VAO = 0;
uint32_t animationVAO = 0;

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
	GLsizei length, const char* message, const void* userParam);

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	spdlog::info("---------------\n");
	spdlog::info("Debug message ({0}): {1}\n", id, message);

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             spdlog::info("Source: API"); ; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   spdlog::info("Source: Window System"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: spdlog::info("Source: Shader Compiler"); break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     spdlog::info("Source: Third Party"); break;
		case GL_DEBUG_SOURCE_APPLICATION:     spdlog::info("Source: Application"); break;
		case GL_DEBUG_SOURCE_OTHER:           spdlog::info("Source: Other"); break;
	}
	spdlog::info("\n");

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               spdlog::info("Type: Error"); break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: spdlog::info("Type: Deprecated Behaviour"); break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  spdlog::info("Type: Undefined Behaviour"); break;
		case GL_DEBUG_TYPE_PORTABILITY:         spdlog::info("Type: Portability"); break;
		case GL_DEBUG_TYPE_PERFORMANCE:         spdlog::info("Type: Performance"); break;
		case GL_DEBUG_TYPE_MARKER:              spdlog::info("Type: Marker"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          spdlog::info("Type: Push Group"); break;
		case GL_DEBUG_TYPE_POP_GROUP:           spdlog::info("Type: Pop Group"); break;
		case GL_DEBUG_TYPE_OTHER:               spdlog::info("Type: Other"); break;
	}
	spdlog::info("\n");

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         spdlog::info("Severity: high"); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       spdlog::info("Severity: medium"); break;
		case GL_DEBUG_SEVERITY_LOW:          spdlog::info("Severity: low"); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: spdlog::info("Severity: notification"); break;
	} 
	spdlog::info("\n\n");
}

void DemoApplication::startup()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	
	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Animation", nullptr, nullptr);

	if (window == nullptr)
	{
		spdlog::info("Failed to create GLFW window.\n");
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetWindowUserPointer(window, this);
	
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		spdlog::info("Failed to initialize GLAD\n");
	}

	glEnable(GL_DEPTH_TEST);

	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
	}

	angle = 0.0f;
	shader = std::make_shared<Shader>("Assets/Shaders/Static.vert.spv", "Assets/Shaders/Lit.frag.spv");
	meshShader = std::make_shared<Shader>("Assets/Shaders/Mesh.vert.spv", "Assets/Shaders/Mesh.frag.spv");
	displayTexture = std::make_shared<Texture>("Assets/Textures/uv.png");

	prepareCubeData();
	prepareAnimationDebugData();
	prepareDebugData();
	
	auto rotation = glm::angleAxis(45.0f * Math::DegreeToRadian, glm::vec3(0.0f, 0.0f, 1.0f));

	auto rotatedVector = rotation * glm::vec3(1.0f, 0.0f, 0.0f);

	auto matrix = glm::toMat4(rotation);

	auto vector = glm::vec3(1.0f, 0.0f, 0.0f);
	auto qVector = glm::vec3(rotation.x, rotation.y, rotation.z);

	auto result = qVector * 2.0f * glm::dot(qVector, vector) +
				  vector * (rotation.w * rotation.w -
				  glm::dot(qVector, qVector)) +
				  glm::cross(qVector, vector) * 2.0f * rotation.w;
}

void DemoApplication::prepareCubeData()
{
	glGenVertexArrays(1, &VAO);

	vertexPositions = std::make_shared<Attribute<Vector3>>();
	vertexNormals = std::make_shared<Attribute<Vector3>>();
	vertexTexCoords = std::make_shared<Attribute<Vector2>>();
	indexBuffer = std::make_shared<IndexBuffer>();

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	std::vector<Vector3> positions = {
		// positions       
		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },

		{ -0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f },

		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },

		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },

		{ -0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f,  0.5f },
		{ -0.5f, -0.5f,  0.5f },
		{ -0.5f, -0.5f, -0.5f },

		{ -0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f, -0.5f }
	};

	vertexPositions->set(positions);

	std::vector<Vector3> normals = {
		// normals       
		{  0.0f,  0.0f, -1.0f },
		{  0.0f,  0.0f, -1.0f },
		{  0.0f,  0.0f, -1.0f },
		{  0.0f,  0.0f, -1.0f },
		{  0.0f,  0.0f, -1.0f },
		{  0.0f,  0.0f, -1.0f },

		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f },

		{ -1.0f,  0.0f,  0.0f },
		{ -1.0f,  0.0f,  0.0f },
		{ -1.0f,  0.0f,  0.0f },
		{ -1.0f,  0.0f,  0.0f },
		{ -1.0f,  0.0f,  0.0f },
		{ -1.0f,  0.0f,  0.0f },

		{  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f },

		{ 0.0f, -1.0f,  0.0f },
		{ 0.0f, -1.0f,  0.0f },
		{ 0.0f, -1.0f,  0.0f },
		{ 0.0f, -1.0f,  0.0f },
		{ 0.0f, -1.0f,  0.0f },
		{ 0.0f, -1.0f,  0.0f },

		{ 0.0f,  1.0f,  0.0f },
		{ 0.0f,  1.0f,  0.0f },
		{ 0.0f,  1.0f,  0.0f },
		{ 0.0f,  1.0f,  0.0f },
		{ 0.0f,  1.0f,  0.0f },
		{ 0.0f,  1.0f,  0.0f }
	};

	vertexNormals->set(normals);

	std::vector<Vector2> uvs = {
		// texture
		{ 0.0f,  0.0f },
		{ 1.0f,  0.0f },
		{ 1.0f,  1.0f },
		{ 1.0f,  1.0f },
		{ 0.0f,  1.0f },
		{ 0.0f,  0.0f },

		{ 0.0f,  0.0f },
		{ 1.0f,  0.0f },
		{ 1.0f,  1.0f },
		{ 1.0f,  1.0f },
		{ 0.0f,  1.0f },
		{ 0.0f,  0.0f },

		{ 1.0f,  0.0f },
		{ 1.0f,  1.0f },
		{ 0.0f,  1.0f },
		{ 0.0f,  1.0f },
		{ 0.0f,  0.0f },
		{ 1.0f,  0.0f },

		{ 1.0f,  0.0f },
		{ 1.0f,  1.0f },
		{ 0.0f,  1.0f },
		{ 0.0f,  1.0f },
		{ 0.0f,  0.0f },
		{ 1.0f,  0.0f },

		{ 0.0f,  1.0f },
		{ 1.0f,  1.0f },
		{ 1.0f,  0.0f },
		{ 1.0f,  0.0f },
		{ 0.0f,  0.0f },
		{ 0.0f,  1.0f },

		{ 0.0f,  1.0f },
		{ 1.0f,  1.0f },
		{ 1.0f,  0.0f },
		{ 1.0f,  0.0f },
		{ 0.0f,  0.0f },
		{ 0.0f,  1.0f }
	};

	vertexTexCoords->set(uvs);

	std::vector<uint32_t> indices = {
		// indices
		0, 1, 2,
		3, 4, 5,

		6, 7, 8,
		9, 10, 11,

		12, 13, 14,
		15, 16, 17,

		18, 19, 20,
		21, 22, 23,

		24, 25, 26,
		27, 28, 29,

		30, 31, 32,
		33, 34, 35
	};

	indexBuffer->set(indices);
}

void DemoApplication::prepareDebugData()
{
	debugDraw = std::make_shared<DebugDraw>();

	debugDraw->Push(Vector3(-0.3f, -0.3f, 0.5f));
	debugDraw->Push(Vector3(0.3f, 0.3f, 0.5f));
	debugDraw->UpdateOpenGLBuffers();
}

void DemoApplication::prepareAnimationDebugData()
{
	glGenVertexArrays(1, &animationVAO);
	
	cgltf_data* data = Loader::loadGLTFFile("Assets/Models/Woman.gltf");

	restPose = Loader::loadRestPose(data);
	animationClips = Loader::loadAnimationClips(data);
	
	restPoseDebugDraw = std::make_shared<DebugDraw>();
	restPoseDebugDraw->FromAnimationPose(restPose);
	restPoseDebugDraw->UpdateOpenGLBuffers();

	currentClip = 7;
	currentPose = restPose;
	currentFrame = 0;
	
	animationClips[currentClip].sample(currentPose, fixTimestep * currentFrame);

	currentPoseDebugDraw = std::make_shared<DebugDraw>();
	currentPoseDebugDraw->FromAnimationPose(currentPose);
	currentPoseDebugDraw->UpdateOpenGLBuffers();

	skeletalMeshs = Loader::loadMeshes(data);
	
	Loader::freeGLTFFile(data);
}

void DemoApplication::shutdown()
{
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

void DemoApplication::update(float deltaTime)
{
	if (bUpdateRotation)
	{
		angle += deltaTime * 45.0f;
	}
	
	//animationClips[currentClip].sample(currentPose, fixTimestep * currentFrame);
	playbackTime = animationClips[currentClip].sample(currentPose, playbackTime + deltaTime);
	currentPoseDebugDraw->FromAnimationPose(currentPose);

	// input
	// -----
	processInput();
}

void DemoApplication::run()
{
	static float simulationTime = 0.0f;

	auto start = std::chrono::steady_clock::now();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		auto now = std::chrono::steady_clock::now();

		std::chrono::duration<double> elapsedSeconds = now - start;

		float realTime = static_cast<float>(elapsedSeconds.count());

		while (simulationTime < realTime)
		{
			simulationTime += fixTimestep;
			update(fixTimestep);
		}

		// render
		// ------
		render();
	}
}

void DemoApplication::render()
{
	// render
	// ------
	glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4 projection = perspective(60.0f, 1.333333333333333f, 0.01f, 1000.0f);
	Matrix4 view = lookAt(eye, center, Vector3::Y);
	
	auto glmView = glm::lookAtRH(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	auto rotation = angleAxis(angle * Math::DegreeToRadian, Vector3::Y);

	Matrix4 model = quaternionToMatrix4(rotation);

	Matrix4 mvp = projection * view * model;

	//model = Matrix4::Identity;

	glBindVertexArray(VAO);
	
	shader->bind();

	//vertexPositions->bindTo(shader->getAttribute("aPosition"));
	//vertexNormals->bindTo(shader->getAttribute("aNormal"));  
	//vertexTexCoords->bindTo(shader->getAttribute("aUV"));

	Uniform<Matrix4>::set(shader->getUniform("model"), model);
	Uniform<Matrix4>::set(shader->getUniform("view"), view);
	Uniform<Matrix4>::set(shader->getUniform("projection"), projection);

	Uniform<Vector3>::set(shader->getUniform("lightDirection"), Vector3(0.0f, 1.0f, 1.0f));

	displayTexture->bind(shader->getUniform("baseTexture"), 0);

	//draw(*indexBuffer.get(), RenderMode::Triangles);
	
	displayTexture->unbind(0);

	//vertexPositions->unbindFrom(shader->getAttribute("aPosition"));
	//vertexNormals->unbindFrom(shader->getAttribute("aNormal"));
	//vertexTexCoords->unbindFrom(shader->getAttribute("aUV"));

	shader->unbind();

	glBindVertexArray(animationVAO);

	meshShader->bind();
	
	rotation = angleAxis(-Math::Degree_90 * Math::DegreeToRadian, Vector3::Y);

	model = quaternionToMatrix4(rotation);
	
	Uniform<Matrix4>::set(shader->getUniform("model"), model);
	Uniform<Matrix4>::set(shader->getUniform("view"), view);
	Uniform<Matrix4>::set(shader->getUniform("projection"), projection);

	Uniform<Vector3>::set(shader->getUniform("lightDirection"), Vector3(0.0f, 1.0f, 1.0f));

	skeletalMeshs[0].bind(0, 1, 2, -1, -1);
	skeletalMeshs[0].draw();

	meshShader->unbind();

	//debugDraw->Draw(DebugDrawMode::Lines, Vector3(1.0f, 0.0f, 0.0f), mvp);
	
	restPoseDebugDraw->Draw(DebugDrawMode::Lines, Vector3(1.0f, 0.0f, 0.0f), mvp);

	currentPoseDebugDraw->UpdateOpenGLBuffers();
	currentPoseDebugDraw->Draw(DebugDrawMode::Lines, Vector3(0, 0, 1), mvp);

	
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void DemoApplication::framebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void DemoApplication::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	spdlog::info("{0}, {1}", xoffset, yoffset);

	auto* app = static_cast<DemoApplication*>(glfwGetWindowUserPointer(window));

	if (yoffset > 0.0f)
	{
		app->eye.z -= 0.1f;
	}
	else if (yoffset < 0.0f)
	{
		app->eye.z += 0.1f;
	}
}

void DemoApplication::keyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
	auto* app = static_cast<DemoApplication*>(glfwGetWindowUserPointer(window));
	
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		app->toggleUpdateRotation();
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		app->currentFrame = (app->currentFrame + 1) % app->animationClips[app->currentClip].getSize();
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		app->currentFrame = (app->currentFrame - 1) % app->animationClips[app->currentClip].getSize();
	}
}

void DemoApplication::processInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void DemoApplication::toggleUpdateRotation()
{
	bUpdateRotation = !bUpdateRotation;
}
