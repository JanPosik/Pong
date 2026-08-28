#include "resource_manager.h"
#include "shader.h"
#include "texture.h"

#include <glad/glad.h>
#include "stb_image.h"

#include <fstream>
#include <sstream>
#include <iostream>

// initialize static variables
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name) {
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char *file, bool alpha, std::string name) {
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Shader ResourceManager::GetShader(std::string name) {
	return Shaders[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
	return Textures[name];
}

void ResourceManager::FreeResources() {
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile) {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	try {
		std::ifstream vertexFile(vShaderFile);
		std::ifstream fragmentFile(fShaderFile);
		std::stringstream vertexStream, fragmentStream;

		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();

		if (gShaderFile != nullptr) {
			std::ifstream geometryFile(gShaderFile);
			std::stringstream geometryStream;
			geometryStream << geometryFile.rdbuf();
			geometryFile.close();
			geometryCode = geometryStream.str();
		}
	}
	catch (std::exception e) {
		std::cerr << "ERROR::SHADER: Failed to read shader source files\n";
	}

	const char* vShaderSource = vertexCode.c_str();
	const char* fShaderSource = fragmentCode.c_str();
	const char* gShaderSource = geometryCode.c_str();

	Shader shader;
	shader.Compile(vShaderSource, fShaderSource, gShaderFile != nullptr ? gShaderSource : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha) {
	Texture2D texture;
	if (alpha) {
		texture.InternalFormat = GL_RGBA;
		texture.ImageFormat = GL_RGBA;
	}

	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

	texture.Generate(width, height, data);
	stbi_image_free(data);
	return texture;
}
