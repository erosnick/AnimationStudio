#define _CRT_SECURE_NO_WARNINGS

#include "DualQuaternionApplication.h"

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
#include <Windows.h>
#include <GLFW/wglext.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <spdlog/spdlog.h>

#include <iostream>
#include <chrono>

constexpr float fixTimestep = 0.016f;

using namespace Animation;

void DualQuaternionApplication::startup()
{
	initGLFW();
	initImGui();
	
	prepareRenderResources();
	prepareAnimationDebugData();
}

void DualQuaternionApplication::initGLFW()
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

void DualQuaternionApplication::initImGui()
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

void DualQuaternionApplication::prepareRenderResources()
{
	linearBlendingSkinningShader = std::make_shared<Shader>("Assets/Shaders/LinearBlendingSkinning.vert.spv", "Assets/Shaders/LinearBlendingSkinning.frag.spv");
	dualQuaternionSkinningShader = std::make_shared<Shader>("Assets/Shaders/DualQuaternionSkinning.vert.spv", "Assets/Shaders/DualQuaternionSkinning.frag.spv");

	//displayTexture = std::make_shared<Texture>("Assets/Textures/uv.png");
	displayTexture = std::make_shared<Texture>("Assets/Models/Woman.png");
}

void DualQuaternionApplication::prepareAnimationDebugData()
{
	glGenVertexArrays(1, &animationVAO);
	
	cgltf_data* data = Loader::loadGLTFFile("Assets/Models/DualQuaternion.gltf");

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
	
	meshes = Loader::loadMeshes(data);

	for (auto& mesh : meshes)
	{
		rearrangeSkeletalMesh(mesh, boneMap);
	}
	
	currentPose = skeleton.getRestPose();

	currentPose.getDualQuaternionPalette(dualQuaternionPosePalette);
	skeleton.getInverseBindPose(dualQuaternionInverseBindPosePalette);

	currentPose.getMatrixPalette(linearBlendingPosePalette);
	linearBlendingInverseBindPosePalette = skeleton.getInverseBindPose();

	currentClip = 0;
	currentFrame = 0;
	
	Loader::freeGLTFFile(data);
}

void DualQuaternionApplication::shutdown()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

void DualQuaternionApplication::update(float deltaTime)
{
	updateAnimationPose(deltaTime);

	updateImGui();

	// input
	// -----
	processInput();
}

void DualQuaternionApplication::updateAnimationPose(float deltaTime)
{
	playbackTime = fastAnimationClips[currentClip].sample(currentPose, playbackTime + deltaTime);
	currentPose.getDualQuaternionPalette(dualQuaternionPosePalette);
	currentPose.getMatrixPalette(linearBlendingPosePalette);
}

void DualQuaternionApplication::run()
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

void DualQuaternionApplication::render()
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

	Transform model(Vector3(2, 2, 0), Quaternion(), Vector3(1, 1, 1));;

	glBindVertexArray(animationVAO);

	dualQuaternionSkinningShader->bind();
	
	Uniform<Matrix4>::set(dualQuaternionSkinningShader->getUniform("model"), transformToMatrix4(model));
	Uniform<Matrix4>::set(dualQuaternionSkinningShader->getUniform("view"), view);
	Uniform<Matrix4>::set(dualQuaternionSkinningShader->getUniform("projection"), projection);
	Uniform<Vector3>::set(dualQuaternionSkinningShader->getUniform("lightDirection"), Vector3(1.0f, 1.0f, 1.0f));

	Uniform<DualQuaternion>::set(dualQuaternionSkinningShader->getUniform("animationPose"), dualQuaternionPosePalette);
	Uniform<DualQuaternion>::set(dualQuaternionSkinningShader->getUniform("inverseBindPose"), dualQuaternionInverseBindPosePalette);
	
	displayTexture->bind(dualQuaternionSkinningShader->getUniform("baseTexture"), 0);

	for (auto i = 0; i < meshes.size(); i++)
	{
		meshes[i].bind(0, 1, 2, 3, 4);
		meshes[i].draw();
		meshes[i].unbind(0, 1, 2, 3, 4);
	}

	displayTexture->unbind(0);

	dualQuaternionSkinningShader->unbind();
	
	linearBlendingSkinningShader->bind();

	model.position.x = -2.0f;
	model.position.y = -2.0f;

	Uniform<Matrix4>::set(linearBlendingSkinningShader->getUniform("model"), transformToMatrix4(model));
	Uniform<Matrix4>::set(linearBlendingSkinningShader->getUniform("view"), view);
	Uniform<Matrix4>::set(linearBlendingSkinningShader->getUniform("projection"), projection);

	Uniform<Vector3>::set(linearBlendingSkinningShader->getUniform("lightDirection"), Vector3(1.0f, 1.0f, 1.0f));

	Uniform<Matrix4>::set(linearBlendingSkinningShader->getUniform("animationPose"), linearBlendingPosePalette);
	Uniform<Matrix4>::set(linearBlendingSkinningShader->getUniform("inverseBindPose"), linearBlendingInverseBindPosePalette);

	if (!bPrecomputeSkin)
	{
		Uniform<Matrix4>::set(linearBlendingSkinningShader->getUniform("inverseBindPose"), skeleton.getInverseBindPose());
	}

	displayTexture->bind(linearBlendingSkinningShader->getUniform("baseTexture"), 0);
		
	for (auto i = 0; i < meshes.size(); i++)
	{
		meshes[i].bind(0, 1, 2, 3, 4);
		meshes[i].draw();
		meshes[i].unbind(0, 1, 2, 3, 4);
	}

	displayTexture->unbind(0);

	linearBlendingSkinningShader->unbind();

	renderImGui();
	
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
}

bool DualQuaternionApplication::WGLExtensionSupported(const std::string& extension)
{
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
	bool swapControlSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

	return swapControlSupported;
}

void DualQuaternionApplication::toggleVSync()
{
	PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = nullptr;
	PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = nullptr;

	if (WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		bVSync = !bVSync;
		
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

		wglSwapIntervalEXT(bVSync);
		
		if (bVSync)
		{
			std::cout << "Enabled vsync\n";
		}
		else
		{
			std::cout << "Disabled vsync\n";
		}
	}
	else
	{ // !swapControlSupported
		std::cout << "WGL_EXT_swap_control not supported\n";
	}
}

void DualQuaternionApplication::onFramebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void DualQuaternionApplication::onMouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (ImGui::GetIO().WantCaptureMouse) {
		return;
	}
	
	spdlog::info("{0}, {1}", xOffset, yOffset);

	auto* app = static_cast<DualQuaternionApplication*>(glfwGetWindowUserPointer(window));

	if (yOffset > 0.0f)
	{
		app->eye.z -= 0.1f;
	}
	else if (yOffset < 0.0f)
	{
		app->eye.z += 0.1f;
	}
}

void DualQuaternionApplication::onKeyCallback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
	auto* app = static_cast<DualQuaternionApplication*>(glfwGetWindowUserPointer(window));
	
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		app->toggleVSync();
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

void DualQuaternionApplication::processInput()
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

void DualQuaternionApplication::updateImGui()
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
		ImGui::Checkbox("VSync", &bVSync);

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

void DualQuaternionApplication::renderImGui()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
