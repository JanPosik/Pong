#ifndef GAME_H
#define GAME_H

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
