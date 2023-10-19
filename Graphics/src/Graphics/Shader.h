#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include "glm/gtc/matrix_transform.hpp"

namespace Graphics
{
	struct ShaderProgramSource {

		std::string vertex, fragment;
	};

	class Shader
	{
	public:
		Shader();
		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void UnBind() const;

		unsigned int GetRendererID();

		// Set uniforms
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform3f(const std::string& name, const glm::vec3 vec);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniformMat3f(const std::string& name, const glm::mat3& matrix);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	private:
		unsigned int m_RendererID;
		std::string m_FilePath;			// for debugging
		std::unordered_map <std::string, int> m_UniformLocationCache;

		int GetUniformLocation(const std::string& name);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		ShaderProgramSource ParseShader(const std::string& filepath);
	};
}