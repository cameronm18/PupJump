#ifndef GAME_H
#define GAME_H

#include <vector>
//#include "GameLevel.h"

#include "ResourceManager.h"
#include "GameObject.h"
#include <iostream>
#include "SaveGame.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//representation of the current game state
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};
// Represents the four possible (collision) directions
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>



//Game class combines all game related data for ease of use
class Game
{
public:
	//game state
	GameState		State;
	bool			Keys[1024];
	unsigned int	Width, Height;
	
	//constructor and deconstruct
	Game(unsigned int width, unsigned int height);
	~Game();

	//init game state
	void Init();

	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	static void onBallBounce(void* context);

	void AddBall(glm::vec2 spawnLoc);

	void ProcessCollisions();
	// reset
	void ResetLevel();
	void ResetPlayer();
	

};
#endif
