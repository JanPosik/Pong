#include "texture.h"
#include <glad/glad.h>

Texture2D::Texture2D()
	: Width(0), Height(0), InternalFormat(GL_RGB), ImageFormat(GL_RGB), WrapS(GL_REPEAT), WrapT(GL_REPEAT), FilterMin(GL_LINEAR), FilterMax(GL_LINEAR) {
	glGenTextures(1, &ID);
}

void Texture2D::Generate(unsigned int width, unsigned int height, const unsigned char* data) {
	Width = width;
	Height = height;

	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, ImageFormat, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterMax);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const {
	glBindTexture(GL_TEXTURE_2D, ID);
}
