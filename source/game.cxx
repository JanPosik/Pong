#include "game.h"
#include "game_object.h"
#include "ball_object.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "particle_generator.h"
#include "post_processor.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

SpriteRenderer* Renderer;
ParticleGenerator* Particles;
GameObject* Player1;
GameObject* Player2;
BallObject* Ball;
PostProcessor* Effects;

float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), Keys(), Width(width), Height(height)
{}

void Game::Free() {
	delete Renderer;
	delete Particles;
	delete Player1;
	delete Player2;
	delete Ball;
	delete Effects;
}

void Game::Init() {
	// load shaders
	ResourceManager::LoadShader("resources/shaders/sprite_vertex.shader", "resources/shaders/sprite_fragment.shader", nullptr, "sprite");
	ResourceManager::LoadShader("resources/shaders/particle_vertex.shader", "resources/shaders/particle_fragment.shader", nullptr, "particle");
	ResourceManager::LoadShader("resources/shaders/postprocessing_vertex.shader", "resources/shaders/postprocessing_fragment.shader", nullptr, "postprocessing");
	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInt("image", 0);
	ResourceManager::GetShader("sprite").SetMat4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetMat4("projection", projection);
	// render specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	// load texture
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("resources/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("resources/textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("resources/textures/particle.png", true, "particle");
	// player
	glm::vec2 player1Pos = glm::vec2(0.0f, Height / 2.0f - PLAYER_SIZE.y / 2.0f);
	glm::vec2 player2Pos = glm::vec2(this->Width - PLAYER_SIZE.x, Height / 2.0f - PLAYER_SIZE.y / 2.0f);
	Player1 = new GameObject(player1Pos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	Player2 = new GameObject(player2Pos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	// ball
	glm::vec2 ballPos = glm::vec2(this->Width / 2.0f - BALL_RADIUS, this->Height / 2.0f - BALL_RADIUS);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
	// particles
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	// effects
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);

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
	// update objects
	Ball->Move(dt, this->Height);
	// check for collisions
	this->DoCollisions();
	// update particles
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));

	// reduce shake time
	if (ShakeTime > 0.0f) {
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = false;
	}

	if (Ball->Position.x <= 0.0f || Ball->Position.x >= this->Width)
		this->Reset();
}

void Game::Render () {
	if (State == GAME_ACTIVE) {
		Effects->BeginRender();
		// background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height));
		Player1->Draw(*Renderer);
		Player2->Draw(*Renderer);
		// render particles before ball, so they're behind
		Particles->Draw();
		Ball->Draw(*Renderer);
		Effects->EndRender();
		Effects->Render(static_cast<float>(glfwGetTime()));
	}
}

void Game::Reset() {
	Player1->Position = glm::vec2(0.0f, this->Height / 2.0f - PLAYER_SIZE.y / 2.0f);
	Player2->Position = glm::vec2(this->Width - PLAYER_SIZE.x, this->Height / 2.0f - PLAYER_SIZE.y / 2.0f);
	int random_diretion_x = (rand() % 2) * 2 - 1;
	int random_diretion_y = (rand() % 2) * 2 - 1;
	Ball->Reset(glm::vec2(this->Width / 2.0f, this->Height / 2.0f), glm::vec2(INITIAL_BALL_VELOCITY.x * random_diretion_x, INITIAL_BALL_VELOCITY.y * random_diretion_y));
}

// collision detection
bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 target);

void Game::DoCollisions() {
	// player collisions
	GameObject* Players[2] = { Player1, Player2 };

	for (unsigned int i = 0; i < 2; i++) {
		Collision result = CheckCollision(*Ball, *Players[i]);
		if (!Ball->Stuck && std::get<0>(result)) {
		        float centerBoard = Players[i]->Position.y + Players[i]->Size.y / 2.0f;
		        float distance = (Ball->Position.y + Ball->Radius) - centerBoard;
        		float percentage = distance / (Players[i]->Size.y / 2.0f);
        		// then move accordingly
        		float strength = -2.0f;
        		glm::vec2 oldVelocity = Ball->Velocity;
        		Ball->Velocity.x = -1.0f * abs((int)Ball->Velocity.x) * (i == 0 ? -1.0f : 1.0f);
        		Ball->Velocity.y = INITIAL_BALL_VELOCITY.y * percentage * strength; 
        		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
			// make harder to hit the ball by fasten it
			Ball->Velocity.x *= 1.1f;

			// do shake
			ShakeTime = 0.05f;
			Effects->Shake = true;
		}
	}
}

bool CheckCollision(GameObject& one, GameObject& two) {
	// collision x-axis
	bool collisionX = one.Position.x <= two.Position.x + two.Size.x && one.Position.x + one.Size.x >= two.Position.x;
	// collision y-axis
	bool collisionY = one.Position.y <= two.Position.y + two.Size.y && one.Position.y + one.Size.y >= two.Position.y;
	// collision on both axes
	return collisionX && collisionY;
}

Collision CheckCollision(BallObject& one, GameObject& two) {
	// center of ball
	glm::vec2 center(one.Position + one.Radius);
	// AABB info (center, half-extents)
	glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
	// difference vector between centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// difference between closest and circle's center, check collision
	difference = closest - center;
	if (glm::length(difference) < one.Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

Direction VectorDirection(glm::vec2 target) {
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f), // down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++) {
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}
