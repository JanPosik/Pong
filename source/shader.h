#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>

class Shader {
public:
	unsigned int ID;

	Shader();
	Shader& Use();
	void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);

	// set uniform functions
	void SetFloat(const char* name, float value);
	void SetInt(const char* name, int value);
	void SetVec2(const char* name, glm::vec2& vector);
	void SetVec2(const char* name, float x, float y);
	void SetVec3(const char* name, glm::vec3& vector);
	void SetVec3(const char* name, float x, float y, float z);
	void SetVec4(const char* name, glm::vec4& vector);
	void SetVec4(const char* name, float x, float y, float z, float w);
	void SetMat4(const char* name, glm::mat4& matrix);
private:
	void checkCompileErrors(unsigned int object, const std::string& type);
};

#endif // SHADER_H
