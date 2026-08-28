#include "shader.h"
#include <glad/glad.h>
#include <iostream>

Shader::Shader() {}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource) {
	unsigned int vertex, fragment, geometry;

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, nullptr);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, nullptr);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	// geometry shader
	if (geometrySource != nullptr) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometrySource, nullptr);
		glCompileShader(geometry);
	checkCompileErrors(fragment, "GEOMETRY");
	}

	// program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometrySource != nullptr)
		glAttachShader(ID, geometry);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	// delete linked shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometrySource != nullptr)
		glDeleteShader(geometry);
}

Shader& Shader::Use() {
	glUseProgram(ID);
	return *this;
}

void Shader::SetFloat(const char* name, float value) {
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::SetInt(const char* name, int value) {
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetVec2(const char* name, glm::vec2& vector) {
	glUniform2f(glGetUniformLocation(ID, name), vector.x, vector.y);
}

void Shader::SetVec2(const char* name, float x, float y) {
	glUniform2f(glGetUniformLocation(ID, name), x, y);
}

void Shader::SetVec3(const char* name, glm::vec3& vector) {
	glUniform3f(glGetUniformLocation(ID, name), vector.x, vector.y, vector.z);
}

void Shader::SetVec3(const char* name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::SetVec4(const char* name, glm::vec4& vector) {
	glUniform4f(glGetUniformLocation(ID, name), vector.x, vector.y, vector.z, vector.w);
}

void Shader::SetVec4(const char* name, float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}

void Shader::SetMat4(const char* name, glm::mat4& matrix) {
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, false, &matrix[0][0]);
}

void Shader::checkCompileErrors(unsigned int object, const std::string& type) {
	int success;
	char infoLog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, nullptr, infoLog);
			std::cerr << "ERROR::SHADER::COMPILE::" << type << ": " << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, nullptr, infoLog);
			std::cerr << "ERROR::PROGRAM::LINK: " << infoLog << std::endl;
		}
	}
}
