#include "Game.h"
#include "SpriteRenderer.h"
#include "BallObject.h"
#include "TextRenderer.h"


// Initial size of the player
const glm::vec2 PLAYER_SIZE(60.0f, 100.0f);
// Initial velocity of the player
const float PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;
//jump logic
float jumptimer = 0.0f;
bool isJumping = false;
//text render
TextRenderer* Text;

// Game-related State data
SpriteRenderer* Renderer; 
GameObject* Player;
int playerLives = 3;
float hitCooldown = 1.0f;
bool allowHit = true;
int highScore = 0;

// Ball state data 
BallObject* Balls[20];
int ballCount = 0;
int totalBounces = 0;
float spawnCooldown = 0.25f;
bool allowSpawn = true;



Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
    delete Renderer;
    delete Player;

    for (int i = 0; i < ballCount; i++)// remove balls on shutdown
    {
        delete Balls[i];
    }

    if (highScore < totalBounces) //set new highschore before program shuts down
    {
        SaveGame::Save("Data/GameData.sav", totalBounces);
    }
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("Shaders/sprite.vs", "Shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("Images/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("Images/background.jpg", false, "background");
    ResourceManager::LoadTexture("Images/sushi.png", true, "player");
    //create text renderer object and load fonts
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("Fonts/ocraext.TTF", 24);
    //check high score from save file
    highScore = SaveGame::Load("Data/GameData.sav");
    // configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f, PLAYER_SIZE.y -200);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
    //instanciate balls to begin game
    this->AddBall(glm::vec2(std::rand() % 200 + 1));
    this->AddBall(glm::vec2(std::rand() % 400 + 1));

}

void Game::Update(float dt)
{
    //handle ball movement 
    for (int i = 0; i < ballCount; i++)
    {
        Balls[i]->Move(dt, this->Width, this->Height);
    }
       
    //collision check
    this->ProcessCollisions();

    //hit cooldown
    if (!allowHit)
    {
        hitCooldown = hitCooldown - dt;

        if (hitCooldown <= 0)
        {
            allowHit = true;
            hitCooldown = 1.0f;
        }
    }

    //spawn cooldown
    if (!allowSpawn)
    {
        spawnCooldown = spawnCooldown - dt;

        if (spawnCooldown <= 0)
        {
            allowSpawn = true;
            spawnCooldown = 0.25f;
        }
    }

    //game over condition
    if (playerLives <= 0)
    {
        ResetLevel();
        ResetPlayer();
    }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = 2*PLAYER_VELOCITY * dt;
        // move player
        if (this->Keys[GLFW_KEY_A]) //LEFT
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])// RIGHT
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
            }
        }

        if (isJumping && Player->Position.y > this->Height - 300)
        {
            Player->Position.y -= velocity;
        }
        if (!isJumping && Player->Position.y < this->Height - 110)
        {
            Player->Position.y += velocity;
        }

        if (this->Keys[GLFW_KEY_SPACE] == GLFW_PRESS )// JUMP
        {
            if (jumptimer < 0.15f)
            { 
                jumptimer = jumptimer += dt;
                isJumping = true;
            }
            else
            {
                isJumping = false;
            }
        }
        else
        { 
            jumptimer = 0;
            isJumping = false;
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);

        //draw text interface
        Text->RenderText("Lives:" + std::to_string(playerLives), 5.0f, 5.0f, 1.0f);
        Text->RenderText("Score:" + std::to_string(totalBounces), 600.0f, 5.0f, 1.0f);
        Text->RenderText("High Score:" + std::to_string(highScore), 255.0f, 5.0f, 1.0f);

        //draw player sprite
        Player->Draw(*Renderer);

        //drawing each ball in array
        for (int i = 0; i < ballCount; i++)
        {
            Balls[i]->Draw(*Renderer);
        }
    }
}

void Game::ResetLevel()
{
    for (int i = 0; i < ballCount; i++)
    {
        ballCount = 0;
        delete Balls[i];
    }

    this->AddBall(glm::vec2(std::rand() % 200 + 1));
    this->AddBall(glm::vec2(std::rand() % 600 + 1));

    

}

void Game::ResetPlayer()
{
    if (totalBounces > highScore) // set new high score
    {
        highScore = totalBounces;
        SaveGame::Save("Data/GameData.sav", totalBounces);

    }

    // reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    playerLives = 3;
    totalBounces = 0;
}

// collision detection
bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);

void Game::ProcessCollisions()
{

    for (int i = 0; i < ballCount; i++)
    {
        for (int j = i + 1; j < ballCount; j++)
        {
         Collision result = CheckCollision(*Balls[i], *Balls[j]);
            if (allowSpawn && std::get<0>(result))
            {
                

            Direction dir = std::get<1>(result);
            glm::vec2 diff_vector = std::get<2>(result);
            if (dir == LEFT || dir == RIGHT) // horizontal collision
            {
               // relocate
               float penetration = Balls[i]->Radius - std::abs(diff_vector.x);
                Balls[i]->Velocity.x = -Balls[i]->Velocity.x; // reverse horizontal velocity
                Balls[j]->Velocity.x = -Balls[j]->Velocity.x;

                if (Balls[i]->AllowSpawn)
                {
                    if (ballCount <= 20)
                    {
                        allowSpawn = false;
                        this->AddBall(Balls[i]->Position + glm::vec2(std::rand() % 100 + 20));
                        Balls[i]->AllowSpawn = false;
                        Balls[i]->Color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }

                }
               if (dir == LEFT)
               {
                   Balls[i]->Position.x += penetration; // move ball to right
               }
               else
               {
                   Balls[i]->Position.x -= penetration; // move ball to left;
               }

            }
            else // vertical collision
            {  
                float penetration = Balls[i]->Radius - std::abs(diff_vector.y);
                Balls[i]->Velocity.y = -Balls[i]->Velocity.y; // reverse vertical velocity
                Balls[j]->Velocity.y = -Balls[j]->Velocity.y; // reverse vertical velocity

                if (Balls[i]->AllowSpawn)
                {
                    if (ballCount <= 20)
                    {
                        this->AddBall(Balls[i]->Position + glm::vec2(std::rand() % 100 + 20));
                        Balls[i]->AllowSpawn = false;
                        Balls[i]->Color = glm::vec3(1.0f, 0.0f, 0.0f);
                    }
                }

                if (dir == UP)
                {
                    Balls[i]->Position.y -= penetration; // move ball bback up
                }

                else
                {
                    Balls[i]->Position.y += penetration; // move ball back down
                }

            }

            }

        }  

    }


    for (int i = 0; i < ballCount; i++)
    {
        Collision result = CheckCollision(*Balls[i], *Player);
        if (allowHit && std::get<0>(result))
        {
            allowHit = false;

            playerLives--;
            //// check where it hit the board, and change velocity based on where it hit the board
             float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
             float distance = (Balls[i]->Position.x + Balls[i]->Radius) - centerBoard;
             float percentage = distance / (Player->Size.x / 2.0f);
            //    // then move accordingly
             float strength = 2.0f;
             glm::vec2 oldVelocity = Balls[i]->Velocity;
             Balls[i]->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
             Balls[i]->Velocity.y = -Balls[i]->Velocity.y;
             Balls[i]->Velocity.x = -Balls[i]->Velocity.x;
             Balls[i]->Velocity.y = -1.0f * abs(Balls[i]->Velocity.y);
            
        }

    }

}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject& one, GameObject& two) // AABB - Circle collision
{
    // get center point circle first 
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // now that we know the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// calculates which direction a vector is facing (N,E,S or W)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Game::AddBall(glm::vec2 spawnLoc)
{
    if (ballCount < 19)
    {
        Balls[ballCount] = new BallObject(spawnLoc, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
        Balls[ballCount]->SetBounceCallback(onBallBounce, this);
        ballCount++;
        std::cout << "Ballcount:"<< ballCount << std::endl;
    }
    else
    {
        std::cout << "Array full cannot add more balls" << std::endl;

    }
}

//function poniter to handle bounce event
void Game::onBallBounce(void* context)
{
    if (context)
    {
        totalBounces++;
    }
}
