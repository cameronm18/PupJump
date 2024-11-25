#include "BallObject.h"

BallObject::BallObject() : GameObject(), Radius(12.5f), AllowSpawn(true), Bounces(0) { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity), Radius(radius), AllowSpawn(true), Bounces(0) { }




glm::vec2 BallObject::Move(float dt, unsigned int window_width, unsigned int window_height)
{
    // if not stuck to player board

    
        // move the ball
        this->Position += this->Velocity * dt;
        // then check if outside window bounds and if so, reverse velocity and restore at correct position
        if (this->Position.x <= 0.0f)
        {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = 0.0f;
            this->Bounce();
        }
        else if (this->Position.x + this->Size.x >= window_width)
        {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = window_width - this->Size.x;
            this->Bounce();
        }
        if (this->Position.y <= 0.0f)
        {
            this->Velocity.y = -this->Velocity.y;
            this->Position.y = 0.0f;
            this->Bounce();
        }
        else if (this->Position.y + this->Size.x >= window_height)
        {
            this->Velocity.y = -this->Velocity.y;
            this->Position.y = window_height - this->Size.y;
            this->Bounce();
        }
    
    return this->Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->Position = position;
    this->Velocity = velocity;

}

void BallObject::SetBounceCallback(BounceCallback callback, void* context)
{
    this->bounceCallback = callback;
    this->callbackContext = context;


}
 

void BallObject::Bounce()
{

    Bounces++;

    if (bounceCallback)
    {
        bounceCallback(callbackContext);
    }

}



