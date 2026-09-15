#include "game.h"
#include "game_object.h"
#include "ball_object.h"
#include "powerup.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"

#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <sstream>

SpriteRenderer* Renderer;
ParticleGenerator* Particles;
GameObject* Player1;
GameObject* Player2;
BallObject* Ball;
PostProcessor* Effects;
TextRenderer* Text;

float ShakeTime = 0.0f;
int Score1 = 0;
int Score2 = 0;
std::string win_text;

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
	ResourceManager::LoadTexture("resources/textures/powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture("resources/textures/powerup_chaos.png", true, "powerup_chaos");
	ResourceManager::LoadTexture("resources/textures/powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture("resources/textures/powerup_increase.png", true, "powerup_increase");
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
	// text
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("resources/fonts/OCRAEXT.TTF", 24);
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
	}
	if (State == GAME_MENU) {
		if (this->Keys[GLFW_KEY_ENTER]) {
			State = GAME_ACTIVE;
			Ball->Stuck = false;
		}
	}
	if (State == GAME_WIN) {
		if (this->Keys[GLFW_KEY_ENTER]) {
			Score1 = Score2 = 0;
			State = GAME_MENU;
			Effects->Chaos = false;
			Ball->Stuck = true;
			this->Keys[GLFW_KEY_ENTER] = false;
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

	UpdatePowerUps(dt);

	// reduce shake time
	if (ShakeTime > 0.0f) {
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = false;
	}

	if (Ball->Position.x + Ball->Size.x <= 0.0f) {
		Score2++;
		this->Reset();
	}
	if (Ball->Position.x >= this->Width) {
		Score1++;
		this->Reset();
	}

	// check win conditions
	if (Score1 == 10 || Score2 == 10) {
		State = GAME_WIN;
		win_text = (Score1 == 10) ? "Player1 won!!!" : "Player2 won!!!";
		Effects->Chaos = true;
		Ball->Stuck = true;
		this->Reset();
	}
}

void Game::Render () {
	if (State == GAME_ACTIVE || State == GAME_MENU || State == GAME_WIN) {
		Effects->BeginRender();
		// background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height));
		Player1->Draw(*Renderer);
		Player2->Draw(*Renderer);
		// render power ups
		for (PowerUp& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		// render particles before ball, so they're behind
		Particles->Draw();
		Ball->Draw(*Renderer);
		Effects->EndRender();
		Effects->Render(static_cast<float>(glfwGetTime()));
		std::stringstream ss; ss << Score1 << " : " << Score2;
		Text->RenderText(ss.str(), this->Width / 2.0f - (ss.str().length() / 2.0f) * 12 - 6, 5.0f, 1.0f);
	}
	if (State == GAME_MENU) {
		Text->RenderText("Press ENTER to start", 490.0f, this->Height / 2.0f + 100.0f, 1.0f);
	}
	if (State == GAME_WIN) {
		Text->RenderText(win_text, 540.0f, this->Height / 2.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to play again or ESC to quit", 420.0f, this->Height / 2.0f + 50.0f, 0.75f);
	}
}

void Game::Reset() {
	Player1->Position = glm::vec2(0.0f, this->Height / 2.0f - PLAYER_SIZE.y / 2.0f);
	Player2->Position = glm::vec2(this->Width - PLAYER_SIZE.x, this->Height / 2.0f - PLAYER_SIZE.y / 2.0f);
	Player1->Size = Player2->Size = PLAYER_SIZE;
	int random_diretion_x = (rand() % 2) * 2 - 1;
	int random_diretion_y = (rand() % 2) * 2 - 1;
	Ball->Reset(glm::vec2(this->Width / 2.0f, this->Height / 2.0f), glm::vec2(INITIAL_BALL_VELOCITY.x * random_diretion_x, INITIAL_BALL_VELOCITY.y * random_diretion_y));
}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);

void Game::UpdatePowerUps(float dt) {
	for (PowerUp& powerUp : this->PowerUps) {
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated) {
			powerUp.Duration -= dt;
			if (powerUp.Duration <= 0.0f) {
				powerUp.Activated = false;
				if (powerUp.Type == "confuse") {
					if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
						Effects->Confuse = false;
				}
				if (powerUp.Type == "chaos") {
					if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
						Effects->Chaos = false;
				}
			}
		}
	}
	// delete non-active destroyed powerups
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type) {
	for (const PowerUp& powerUp : powerUps) {
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}

bool ShouldSpawn(unsigned int chance) {
	unsigned int random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps() {
	glm::vec2 position = glm::vec2(this->Width / 2.0f - POWER_UP_SIZE.x / 2.0f, 0.0f);
	if (ShouldSpawn(75)) {
		position.y = rand() % (this->Height - (int)POWER_UP_SIZE.y);
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, position, ResourceManager::GetTexture("powerup_speed")));
	}
	if (ShouldSpawn(75)) {
		position.y = rand() % (this->Height - (int)POWER_UP_SIZE.y);
		this->PowerUps.push_back(PowerUp("increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, position, ResourceManager::GetTexture("powerup_increase")));
	}
	if (ShouldSpawn(15)) {
		position.y = rand() % (this->Height - (int)POWER_UP_SIZE.y);
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, position, ResourceManager::GetTexture("powerup_confuse")));
	}
	if (ShouldSpawn(15)) {
		position.y = rand() % (this->Height - (int)POWER_UP_SIZE.y);
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, position, ResourceManager::GetTexture("powerup_chaos")));
	}
}

void ActivatePowerUp(PowerUp& powerUp, GameObject& player) {
	if (powerUp.Type == "speed")
		Ball->Velocity *= 1.2f;
	else if (powerUp.Type == "increase")
		player.Size.y += 50.0f;
	else if (powerUp.Type == "confuse") {
		if (!Effects->Chaos)
			Effects->Confuse = true;
	}
	else if (powerUp.Type == "chaos") {
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
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
			Ball->Velocity.x *= 1.05f;

			// do shake
			ShakeTime = 0.05f;
			Effects->Shake = true;

			// spawn powerups
			SpawnPowerUps();
		}
	}

	for (PowerUp& powerUp : this->PowerUps) {
		if (!powerUp.Destroyed) {
			if (powerUp.Position.x <= 0.0f - POWER_UP_SIZE.x || powerUp.Position.x >= this->Width)
				powerUp.Destroyed = true;
			for (unsigned int i = 0; i < 2; i++) {
				if (CheckCollision(*Players[i], powerUp)) {
					ActivatePowerUp(powerUp, *Players[i]);
					powerUp.Destroyed = true;
					powerUp.Activated = true;
				}
			}
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
