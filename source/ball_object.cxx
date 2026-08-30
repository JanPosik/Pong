#include "ball_object.h"
#include "game_object.h"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

BallObject::BallObject()
	: GameObject(), Radius(12.0f), Stuck(true) {}

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2.0f), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true) {}

glm::vec2 BallObject::Move(float dt, unsigned int window_height) {
	if (!Stuck) {
		Position += Velocity * dt;

		// top
		if (Position.y <= 0.0f) {
			Velocity.y = -Velocity.y;
			Position.y = 0.0f;
		}
		else if (Position.y >= window_height - Size.y) {
			Velocity.y = -Velocity.y;
			Position.y = window_height - Size.y;
		}
	}

	return Position;
}

void BallObject::Reset(glm::vec2 positon, glm::vec2 velocity) {
	Position = positon;
	Velocity = velocity;
	Stuck = true;
}
