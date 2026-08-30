#include "game.h"
#include "game_object.h"
#include "ball_object.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

SpriteRenderer* Renderer;
GameObject* Player1;
GameObject* Player2;
BallObject* Ball;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), Keys(), Width(width), Height(height)
{}

Game::~Game() {
	delete Renderer;
	delete Player1;
	delete Player2;
	delete Ball;
}

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
	ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
	// player
	glm::vec2 player1Pos = glm::vec2(0.0f, Height / 2.0f - PLAYER_SIZE.y / 2.0f);
	glm::vec2 player2Pos = glm::vec2(this->Width - PLAYER_SIZE.x, Height / 2.0f - PLAYER_SIZE.y / 2.0f);
	Player1 = new GameObject(player1Pos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	Player2 = new GameObject(player2Pos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	// ball
	glm::vec2 ballPos = glm::vec2(this->Width / 2.0f - BALL_RADIUS, this->Height / 2.0f - BALL_RADIUS);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

	State = GAME_ACTIVE;
}

void Game::ProcessInput(float dt) {
	if (State == GAME_ACTIVE) {
		float velocity = PLAYER_VELOCITY * dt;
		if (this->Keys[GLFW_KEY_W]) {
			if (Player1->Position.y >= 0.0f)
				Player1->Position.y -= velocity;
		}
		if (this->Keys[GLFW_KEY_S]) {
			if (Player1->Position.y <= this->Height - Player1->Size.y)
				Player1->Position.y += velocity;
		}
		if (this->Keys[GLFW_KEY_UP]) {
			if (Player2->Position.y >= 0.0f)
				Player2->Position.y -= velocity;
		}
		if (this->Keys[GLFW_KEY_DOWN]) {
			if (Player2->Position.y <= this->Height - Player2->Size.y)
				Player2->Position.y += velocity;
		}
		if (this->Keys[GLFW_KEY_SPACE]) {
			Ball->Stuck = false;
		}
	}
}

void Game::Update(float dt) {
	Ball->Move(dt, this->Height);
}

void Game::Render () {
	if (State == GAME_ACTIVE) {
		Player1->Draw(*Renderer);
		Player2->Draw(*Renderer);
		Ball->Draw(*Renderer);
	}
}
