#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

namespace Renderer
{
	
	Shader::Shader()
	{
		handle = glCreateProgram();
	}

	Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		handle = glCreateProgram();
		load(vertexShaderPath, fragmentShaderPath);
	}

	Shader::~Shader()
	{
		glDeleteProgram(handle);
	}

	std::string Shader::readFile(const std::string& path)
	{
		std::ifstream file;
		// ensure ifstream objects can throw exceptions:
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		
		file.open(path);
		
		std::stringstream contents;
		contents << file.rdbuf();
		
		file.close();
		
		return contents.str();
	}

	uint32_t Shader::compileShader(const std::string& shaderSource, int32_t shaderType)
	{
		uint32_t id = glCreateShader(shaderType);

		const char* source = shaderSource.c_str();

		glShaderSource(id, 1, &source, nullptr);
		glCompileShader(id);

		int32_t success = 0;

		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(id, 512, nullptr, infoLog);
			std::cout << "Error: Shader compilation failed: \n" << infoLog << std::endl;
			glDeleteProgram(id);
			return 0;
		}

		return id;
	}

	bool Shader::linkShaders(uint32_t vertexShader, uint32_t fragmentShader)
	{
		glAttachShader(handle, vertexShader);
		glAttachShader(handle, fragmentShader);
		glLinkProgram(handle);
		
		int32_t success = 0;

		glGetProgramiv(handle, GL_LINK_STATUS, &success);

		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(handle, 512, nullptr, infoLog);
			std::cout << "Error: Program linking failed: \n" << infoLog << std::endl;
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return false;	
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return true;
	}

	void Shader::populateAttributes()
	{
		glUseProgram(handle);

		int32_t count = -1;
		glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &count);

		char name[128] = {};
		int32_t length = 0;
		int32_t size = 0;
		GLenum type;
		
		for (int32_t i = 0; i < count; i++)
		{
			memset(name, 0, sizeof(char) * 128);
			glGetActiveAttrib(handle, (GLuint)i, 128, &length, &size, &type, name);
			int32_t attributeLocation = glGetAttribLocation(handle, name);

			// Is attribute valid?
			if (attributeLocation >= 0)
			{
				attributes[name] = attributeLocation;
			}
		}

		glUseProgram(0);
	}

	void Shader::populateUniforms()
	{
		glUseProgram(handle);

		int32_t count = -1;
		glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);

		char name[128] = {};
		char testName[256] = {};
		int32_t length = 0;
		int32_t size = 0;
		GLenum type;

		for (int32_t i = 0; i < count; i++)
		{
			memset(name, 0, sizeof(char) * 128);
			glGetActiveUniform(handle, (GLuint)i, 128, &length, &size, &type, name);
			
			int32_t uniformLocation = glGetUniformLocation(handle, name);

			// Is uniform valid?
			if (uniformLocation >= 0)
			{
				std::string uniformName = name;

				// If name contains [, uniform is array
				std::size_t found = uniformName.find('[');

				if (found != std::string::npos)
				{
					uniformName.erase(uniformName.begin() + found, uniformName.end());

					uint32_t uniformIndex = 0;

					while (true)
					{
						memset(testName, 0, sizeof(char) * 256);
						sprintf_s(testName, "%s[%d]", uniformName.c_str(), uniformIndex++);

						int32_t uniformLocation = glGetUniformLocation(handle, testName);

						if (uniformLocation < 0)
						{
							break;
						}
						uniforms[testName] = uniformLocation;
					}
				}
				uniforms[testName] = uniformLocation;
			}
		}
		glUseProgram(0);
	}

	void Shader::load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		std::string vertexShaderSource = readFile(vertexShaderPath);
		std::string fragmentShaderSource = readFile(fragmentShaderPath);

		uint32_t vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
		uint32_t fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

		if (linkShaders(vertexShader, fragmentShader))
		{
			populateAttributes();
			populateUniforms();
		}
	}

	void Shader::bind()
	{
		glUseProgram(handle);
	}

	void Shader::unbind()
	{
		glUseProgram(0);
	}

	uint32_t Shader::getAttribute(const std::string& name)
	{
		auto iterator = attributes.find(name);

		if (iterator == attributes.end())
		{
			std::cout << "Bad attribute index: " << name << "\n";
			return 0;
		}

		return iterator->second;
	}

	uint32_t Shader::getUniform(const std::string& name)
	{
		auto iterator = uniforms.find(name);

		if (iterator == uniforms.end())
		{
			std::cout << "Bad uniform index: " << name << "\n";
			return 0;
		}

		return iterator->second;
	}
}