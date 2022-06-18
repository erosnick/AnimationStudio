#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Debug
{
	char* convert(const std::string& inString);
		
	void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
		GLsizei length, const char* message, const void* userParam);
}