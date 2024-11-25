#pragma once
#include "GameObject.h"
#include <iostream>


class BallObject : public GameObject
{
public:


	//ball state
	float Radius;
	bool AllowSpawn;
	int Bounces;

	using BounceCallback = void (*)(void* context);

	void SetBounceCallback(BounceCallback callback, void* context);
	void Bounce();

	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2 Move(float dt, unsigned int window_width, unsigned int window_height);
	void Reset(glm::vec2 position, glm::vec2 velocity);



private:
	BounceCallback bounceCallback = nullptr;
	void* callbackContext = nullptr;


};

