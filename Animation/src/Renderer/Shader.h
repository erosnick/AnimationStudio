#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace Renderer
{
	class Shader
	{
	public:
		Shader();
		Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		~Shader();

		void load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		void loadSPIRV(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

		void bind();
		void unbind();

		uint32_t getAttribute(const std::string& name);
		uint32_t getUniform(const std::string& name);
		inline uint32_t getHandle() const { return handle; }

		Shader(const Shader& shader) = delete;
		Shader& operator=(const Shader& shader) = delete;
		
		std::string readFile(const std::string& path);
		std::vector<char> readFileSPIRV(const std::string& path);
		uint32_t compileShader(const std::string& shaderSource, int32_t shaderType);
		uint32_t compileShaderSPIRV(const std::vector<char>& shaderSource, int32_t shaderType);
		bool linkShaders(uint32_t vertexShader, uint32_t fragmentShader);

		void populateAttributes();
		void populateUniforms();
		
	private:
		unsigned int handle;

		std::map<std::string, uint32_t> attributes;
		std::map<std::string, uint32_t> uniforms;
	};
}