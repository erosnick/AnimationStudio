#define _CRT_SECURE_NO_WARNINGS

#include "DemoApplication.h"

#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Math.h"

#include "Renderer/Uniform.h"
#include "Renderer/Renderer.h"
#include "Loader/GLTFLoader.h"
#include "Utils/Timer.h"
#include "Utils/Debug.h"

#include "Animation/RearrangeBones.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <UI/imgui/imgui.h>
#include <UI/imgui/imgui_impl_glfw.h>
#include <UI/imgui/imgui_impl_opengl3.h>
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

void DemoApplication::startup()
{
	initGLFW();
	initImGui();

	angle = 0.0f;
	
	prepareRenderResources();
	prepareCubeData();
	prepareAnimationDebugData();
	prepareDebugData();
}

void DemoApplication::initGLFW()
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
	glfwSetFramebufferSizeCallback(window, onFramebufferSizeCallback);
	glfwSetScrollCallback(window, onMouseScrollCallback);
	glfwSetKeyCallback(window, onKeyCallback);
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
		glDebugMessageCallback(Debug::glDebugOutput, nullptr);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
	}
}

void DemoApplication::initImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImGui::GetStyle().ScaleAllSizes(1.0f);
	
	// GL 3.0 + GLSL 130
	const char* glslVersion = "#version 130";
	
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
}

void DemoApplication::prepareRenderResources()
{
	shader = std::make_shared<Shader>("Assets/Shaders/Static.vert.spv", "Assets/Shaders/Lit.frag.spv");
	meshShader = std::make_shared<Shader>("Assets/Shaders/Mesh.vert.spv", "Assets/Shaders/Mesh.frag.spv");

	if (bPrecomputeSkin)
	{
		skinnedMeshShader = std::make_shared<Shader>("Assets/Shaders/PrecomputeSkinnedMesh.vert.spv", "Assets/Shaders/PrecomputeSkinnedMesh.frag.spv");
	}
	else
	{
		skinnedMeshShader = std::make_shared<Shader>("Assets/Shaders/SkinnedMesh.vert.spv", "Assets/Shaders/SkinnedMesh.frag.spv");
	}

	//displayTexture = std::make_shared<Texture>("Assets/Textures/uv.png");
	displayTexture = std::make_shared<Texture>("Assets/Models/Woman.png");
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
	//cgltf_data* data = Loader::loadGLTFFile("Assets/Models/suzanne.gltf");
	//cgltf_data* data = Loader::loadGLTFFile("Assets/Models/mech-drone.gltf");

	skeleton = Loader::loadSkeleton(data);

	BoneMap boneMap = rearrangeSkeleton(skeleton);

	std::vector<AnimationClip> animationClips = Loader::loadAnimationClips(data);
	fastAnimationClips.resize(animationClips.size());

	for (auto i = 0; i < animationClips.size(); i++)
	{
		rearrangeAnimationClip(animationClips[i], boneMap);
		fastAnimationClips[i] = optimizeAnimationClip(animationClips[i]);
		animationNames.emplace_back(animationClips[i].getName());
	}

	std::transform(animationNames.begin(), animationNames.end(), std::back_inserter(animationNamesArray), Debug::convert);
	
	CPUSkinnedMeshes = Loader::loadMeshes(data);

	for (auto& mesh : CPUSkinnedMeshes)
	{
		rearrangeSkeletalMesh(mesh, boneMap);
	}

	GPUSkinnedMeshes = CPUSkinnedMeshes;
	
	AnimationPose restPose = skeleton.getRestPose();
	AnimationPose bindPose = skeleton.getBindPose();

	GPUAnimationInfo.animationPose = restPose;
	GPUAnimationInfo.animationPosePalette.resize(restPose.getSize());
	CPUAnimationInfo.animationPose = restPose;
	CPUAnimationInfo.animationPosePalette.resize(restPose.getSize());
	
	//CPUSkinnedMeshes[0].hasAnimation() = false;
	//GPUSkinnedMeshes[0].hasAnimation() = false;

	currentPoseDebugDraw = std::make_shared<DebugDraw>();
	
	if (CPUSkinnedMeshes[0].hasAnimation())
	{
		CPUSkinnedMeshes[0].CPUSkinUseMatrixPalette(skeleton, bindPose);

		restPoseDebugDraw = std::make_shared<DebugDraw>();
		restPoseDebugDraw->FromAnimationPose(CPUAnimationInfo.animationPose);
		restPoseDebugDraw->UpdateOpenGLBuffers();

		currentPoseDebugDraw->FromAnimationPose(CPUAnimationInfo.animationPose);
		currentPoseDebugDraw->UpdateOpenGLBuffers();
	}

	currentClip = 0;
	currentFrame = 0;
	
	Loader::freeGLTFFile(data);
}

void DemoApplication::shutdown()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
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
	
	updateAnimationPose(deltaTime);

	if (bPrecomputeSkin)
	{
		updatePrecomputedCPUSkin();
		updatePrecomputedGPUSkin();
	}
	else
	{
		updateCPUSkin();
		updateGPUSkin();
	}

	currentPoseDebugDraw->FromAnimationPose(CPUAnimationInfo.animationPose);

	updateImGui();

	// input
	// -----
	processInput();
}

void DemoApplication::updateAnimationPose(float deltaTime)
{
	if (CPUSkinnedMeshes[0].hasAnimation())
	{
		CPUAnimationInfo.time = fastAnimationClips[currentClip].sample(CPUAnimationInfo.animationPose, CPUAnimationInfo.time + deltaTime);
		GPUAnimationInfo.time = fastAnimationClips[currentClip].sample(GPUAnimationInfo.animationPose, GPUAnimationInfo.time + deltaTime);
	}
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

		//while (simulationTime < realTime)
		//{
		//	simulationTime += fixTimestep;
		//	update(fixTimestep);
		//}

		update(fixTimestep);

		// render
		// ------
		render();
	}
}

void DemoApplication::render()
{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	glfwPollEvents();
	
	// 	
	// render
	// ------
	//glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
	glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4 projection = perspective(60.0f, static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.01f, 1000.0f);
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

	Uniform<Vector3>::set(shader->getUniform("lightDirection"), Vector3(1.0f, 1.0f, 1.0f));

	displayTexture->bind(shader->getUniform("baseTexture"), 0);

	//draw(*indexBuffer.get(), RenderMode::Triangles);
	
	displayTexture->unbind(0);

	//vertexPositions->unbindFrom(shader->getAttribute("aPosition"));
	//vertexNormals->unbindFrom(shader->getAttribute("aNormal"));
	//vertexTexCoords->unbindFrom(shader->getAttribute("aUV"));

	shader->unbind();

	glBindVertexArray(animationVAO);

	meshShader->bind();

	model.m03 += 2.5f;
	
	Uniform<Matrix4>::set(meshShader->getUniform("model"), model);
	Uniform<Matrix4>::set(meshShader->getUniform("view"), view);
	Uniform<Matrix4>::set(meshShader->getUniform("projection"), projection);

	Uniform<Vector3>::set(meshShader->getUniform("lightDirection"), Vector3(1.0f, 1.0f, 1.0f));

	displayTexture->bind(meshShader->getUniform("baseTexture"), 0);

	for (auto i = 0; i < GPUSkinnedMeshes.size(); i++)
	{
		CPUSkinnedMeshes[i].bind(0, 1, 2, -1, -1);
		CPUSkinnedMeshes[i].draw();
		CPUSkinnedMeshes[i].unbind(0, 1, 2, -1, -1);
	}

	displayTexture->unbind(0);

	meshShader->unbind();
	
	if (GPUSkinnedMeshes[0].hasAnimation())
	{
		skinnedMeshShader->bind();

		model.m03 -= 5.5f;

		Uniform<Matrix4>::set(skinnedMeshShader->getUniform("model"), model);
		Uniform<Matrix4>::set(skinnedMeshShader->getUniform("view"), view);
		Uniform<Matrix4>::set(skinnedMeshShader->getUniform("projection"), projection);

		Uniform<Vector3>::set(skinnedMeshShader->getUniform("lightDirection"), Vector3(1.0f, 1.0f, 1.0f));

		Uniform<Matrix4>::set(skinnedMeshShader->getUniform("animationPose"), GPUAnimationInfo.animationPosePalette);

		if (!bPrecomputeSkin)
		{
			Uniform<Matrix4>::set(skinnedMeshShader->getUniform("inverseBindPose"), skeleton.getInverseBindPose());
		}

		displayTexture->bind(skinnedMeshShader->getUniform("baseTexture"), 0);

		//debugDraw->Draw(DebugDrawMode::Lines, Vector3(1.0f, 0.0f, 0.0f), mvp);
		
		for (auto i = 0; i < GPUSkinnedMeshes.size(); i++)
		{
			GPUSkinnedMeshes[i].bind(0, 1, 2, 3, 4);
			GPUSkinnedMeshes[i].draw();
			GPUSkinnedMeshes[i].unbind(0, 1, 2, 3, 4);
		}

		displayTexture->unbind(0);

		skinnedMeshShader->unbind();

		restPoseDebugDraw->Draw(DebugDrawMode::Lines, Vector3(1.0f, 0.0f, 0.0f), mvp);

		currentPoseDebugDraw->UpdateOpenGLBuffers();
		currentPoseDebugDraw->Draw(DebugDrawMode::Lines, Vector3(0, 0, 1), mvp);		
	}

	renderImGui();
	
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
}  

void DemoApplication::onFramebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void DemoApplication::onMouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (ImGui::GetIO().WantCaptureMouse) {
		return;
	}
	
	spdlog::info("{0}, {1}", xOffset, yOffset);

	auto* app = static_cast<DemoApplication*>(glfwGetWindowUserPointer(window));

	if (yOffset > 0.0f)
	{
		app->eye.z -= 0.1f;
	}
	else if (yOffset < 0.0f)
	{
		app->eye.z += 0.1f;
	}
}

void DemoApplication::onKeyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
	auto* app = static_cast<DemoApplication*>(glfwGetWindowUserPointer(window));
	
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		app->toggleUpdateRotation();
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		app->currentFrame = (app->currentFrame + 1) % app->fastAnimationClips[app->currentClip].getSize();
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		app->currentFrame = (app->currentFrame - 1) % app->fastAnimationClips[app->currentClip].getSize();
	}
}

void DemoApplication::processInput()
{
	if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
	{
		return;
	}
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void DemoApplication::toggleUpdateRotation()
{
	bUpdateRotation = !bUpdateRotation;
}

void DemoApplication::updateCPUSkin()
{
	for (auto i = 0; i < CPUSkinnedMeshes.size(); i++)
	{
		if (CPUSkinnedMeshes[i].hasAnimation())
		{
			//Util::Timer timer;
			CPUSkinnedMeshes[i].CPUSkinUseMatrixPalette(skeleton, CPUAnimationInfo.animationPose);
		}
	}
}

void DemoApplication::updatePrecomputedCPUSkin()
{
	CPUAnimationInfo.animationPose.getMatrixPalette(CPUAnimationInfo.animationPosePalette);

	std::vector<Matrix4> inverseBindPose = skeleton.getInverseBindPose();

	for (int32_t i = 0; i < CPUAnimationInfo.animationPosePalette.size(); i++)
	{
		CPUAnimationInfo.animationPosePalette[i] = CPUAnimationInfo.animationPosePalette[i] * inverseBindPose[i];
	}

	for (auto i = 0; i < CPUSkinnedMeshes.size(); i++)
	{
		//Util::Timer timer;
		CPUSkinnedMeshes[i].CPUSkinUseMatrixPalette(CPUAnimationInfo.animationPosePalette);
	}
}

void DemoApplication::updateGPUSkin()
{
	GPUAnimationInfo.animationPose.getMatrixPalette(GPUAnimationInfo.animationPosePalette);
}

void DemoApplication::updatePrecomputedGPUSkin()
{
	GPUAnimationInfo.animationPose.getMatrixPalette(GPUAnimationInfo.animationPosePalette);
	std::vector<Matrix4> inverseBindPose = skeleton.getInverseBindPose();

	for (int32_t i = 0; i < GPUAnimationInfo.animationPosePalette.size(); i++)
	{
		GPUAnimationInfo.animationPosePalette[i] = GPUAnimationInfo.animationPosePalette[i] * inverseBindPose[i];
	}
}

void DemoApplication::updateImGui()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (bShowDemoWindow)
		ImGui::ShowDemoWindow(&bShowDemoWindow);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &bShowDemoWindow);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &bShowAnotherWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::SetNextItemWidth(100);

		ImGui::NewLine();

		ImGui::Text("%s%s", "Animations", "(cm)");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(100);
		if (!animationNames.empty())
		{
			ImGui::Combo("  ", &currentClip, &animationNamesArray[0], static_cast<int32_t>(animationNames.size()));
		}
		ImGui::PushItemWidth(100);

		if (!animationNames.empty())
		{
			ImGui::Text("Playing:%s", animationNames[currentClip].c_str());
		}
		
		ImGui::End();
	}

	// 3. Show another simple window.
	if (bShowAnotherWindow)
	{
		ImGui::Begin("Another Window", &bShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			bShowAnotherWindow = false;
		ImGui::End();
	}
}

void DemoApplication::renderImGui()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
