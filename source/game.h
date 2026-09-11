#ifndef GAME_H
#define GAME_H

#include "powerup.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tuple>
#include <vector>

// initial size of player paddle
const glm::vec2 PLAYER_SIZE(20.0f, 100.0f);
// initial velocity of player paddle
const float PLAYER_VELOCITY(500.0f);

// initial ball velocity
const glm::vec2 INITIAL_BALL_VELOCITY(300.0f, -150.0f);
// radius of ball
const float BALL_RADIUS(14.0f);

enum GameState {
	GAME_MENU,
	GAME_ACTIVE,
	GAME_WIN,
	GAME_LOSS
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game {
public:
	// game state
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;
	std::vector<PowerUp> PowerUps;
	// constructor, desctuctor
	Game(unsigned int width, unsigned int height);
	// loads shaders, textures
	void Init();
	// game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	// free resources
	void Free();
	// collisions
	void DoCollisions();
	// reset
	void Reset();
	// power ups
	void SpawnPowerUps();
	void UpdatePowerUps(float dt);
};

#endif
