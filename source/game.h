#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// initial size of player paddle
const glm::vec2 PLAYER_SIZE(20.0f, 100.0f);
// initial velocity of player paddle
const float PLAYER_VELOCITY(500.0f);

// initial ball velocity
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// radius of ball
const float BALL_RADIUS(12.5f);

enum GameState {
	GAME_MENU,
	GAME_ACTIVE,
	GAME_WIN,
	GAME_LOSS
};

class Game {
public:
	// game state
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;
	// constructor, desctuctor
	Game(unsigned int width, unsigned int height);
	~Game();
	// loads shaders, textures
	void Init();
	// game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
};

#endif
