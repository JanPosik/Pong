#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include "game_object.h"
#include "texture.h"
#include <glm/ext/vector_float2.hpp>

class BallObject : public GameObject {
public:
	float Radius;
	bool Stuck;

	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2 Move(float dt, unsigned int window_height);
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif // BALL_OBJECT_H
