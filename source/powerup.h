#ifndef POWER_UP_H
#define POWER_UP_H

#include "game_object.h"
#include "texture.h"
#include <glm/ext/vector_float2.hpp>
#include <string>

const glm::vec2 POWER_UP_SIZE(60.0f, 20.0f);
const float POWER_UP_SPEED(150.0f);

class PowerUp : public GameObject {
public:
	std::string Type;
	float Duration;
	bool Activated;
	bool Destroyed;

	PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
		: GameObject(position, POWER_UP_SIZE, texture, color), Type(type), Duration(duration), Activated(), Destroyed(false) {
		this->Velocity.x = ((rand() % 2) * 2 - 1) * POWER_UP_SPEED;
	}
};

#endif // POWER_UP_H
