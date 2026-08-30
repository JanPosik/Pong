#include "game_object.h"
#include "sprite_renderer.h"

GameObject::GameObject()
	: Position(0.0f, 0.0f), Size(10.0f, 10.0f), Velocity(0.0f, 0.0f), Color(1.0f), Rotation(0.0f), Sprite() {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
	: Position(pos), Size(size), Sprite(sprite), Color(color), Velocity(velocity), Rotation(0.0f) {}

void GameObject::Draw(SpriteRenderer& renderer) {
	renderer.DrawSprite(Sprite, Position, Size, Rotation, Color);
}
