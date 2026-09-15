#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "shader.h"
#include <map>
#include <glm/ext/vector_int2.hpp>

struct Character {
	unsigned int TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

class TextRenderer {
public:
	// list of pre-compiled characters
	std::map<char, Character> Characters;
	// shader for rendering text
	Shader TextShader;
	// constructor
	TextRenderer(unsigned int width, unsigned int height);
	// pre-compiles a set of characters of given fonts
	void Load(std::string font, unsigned int fontSize);
	// renders a text
	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
private:
	// render state
	unsigned int VAO, VBO;
};

#endif // TEXT_RENDERER_H
