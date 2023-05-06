#define _CRT_SECURE_NO_WARNINGS

#include "Debug.h"

#include <string>
#include <spdlog/spdlog.h>

namespace Debug
{
	char* convert(const std::string& inString)
	{
		char* newString = new char[inString.size() + 1];
		std::strcpy(newString, inString.c_str());
		return newString;
	}
	
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
}