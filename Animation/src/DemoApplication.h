#pragma once

#include "Application.h"

struct GLFWwindow;

class DemoApplication : public Application
{
public:
	void startup() override;
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
};