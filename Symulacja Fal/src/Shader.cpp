#include <GL\glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Shader.h"
#include "Renderer.h"


Shader::Shader(const std::string filepath): m_FilePath(filepath), m_RendererID(0) {
	//urucjmianie osobnego programu shader do rysowania
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}
Shader::~Shader() {
	GLCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
	//czytanie kodu dla rozsze�enia .shader
	std::ifstream stream(filepath);
	enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}
	return{ ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
	//kompilacja przed uruchomieniem dla sprawdzenia b��d�w
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Filed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader !\n";
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	//two�enie instancji shader dla openGL i pod��czanie pod g��wny program
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

void Shader::Bind() const {
	GLCall(glUseProgram(m_RendererID));
}
void Shader::Unbind() const {
	GLCall(glUseProgram(0));
}
//Uniformy do przesy�ania danych do shadera
void Shader::SetUniform1i(const std::string & name, int v0) {
	glUniform1i(GetUniformLocation(name), v0);
}
void Shader::SetUniform1f(const std::string & name, float v0) {
	GLCall(glUniform1f(GetUniformLocation(name), v0));
}
void Shader::SetUniform2f(const std::string & name, float v0, float v1) {
	GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}
void Shader::SetUniform3f(const std::string & name, float v0, float v1, float v2) {
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}
void Shader::SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3) {
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniformMat4f(const std::string & name, const glm::mat4& matrix) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}
//pozyskanie nazwy uniform�w z programu shadera
int Shader::GetUniformLocation(const std::string & name) {
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesnt exist" << std::endl;

	m_UniformLocationCache[name] = location;
	return location;
}
