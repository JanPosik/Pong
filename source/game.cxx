#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer* Renderer;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), Keys(), Width(width), Height(height)
{}

Game::~Game() {}

void Game::Init() {
	// load shaders
	ResourceManager::LoadShader("resources/shaders/sprite_vertex.shader", "resources/shaders/sprite_fragment.shader", nullptr, "sprite");
	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInt("image", 0);
	ResourceManager::GetShader("sprite").SetMat4("projection", projection);
	// render specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	// load texture
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face");
}

void Game::ProcessInput(float dt) {}

void Game::Update(float dt) {}

void Game::Render () {
	Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2((float)Width/2-150.0f, (float)Height/2-150.0f), glm::vec2(300.0f, 300.0f), 16.0f, glm::vec3(1.0f, 0.3f, 0.0f));
}
