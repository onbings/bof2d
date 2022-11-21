/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof2d game engine object
 *
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Nov 13 2022  BHA : Initial release
 */
#include <bof2d/bof2d_game_engine.h>
#include <bof2d/bof2d_res_mgr.h>
#include <bof2d/bof2d_sprite_renderer.h>
#include <bof2d/bof2d_game_obj.h>
#include <bof2d/bof2d_ball_obj.h>
#include <bof2d/bof2d_text_renderer.h>
#include <bof2d/bof2d_particle_gen.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>

BEGIN_BOF2D_NAMESPACE()
// Game-related State data
SpriteRenderer* Renderer;
BofBallObject2d* Ball;
TextRenderer* Text;
GameObject* Player;
ParticleGenerator* Particles;
ma_engine* pAudioEngine;
ma_sound music;
ma_sound bleep;
ma_sound solid;


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

Collision CheckCollision(BofBallObject2d& one, GameObject& two) // AABB - Circle collision
{
  // get center point circle first 
  glm::vec2 center(one.Position + one.Radius);
  // calculate AABB info (center, half-extents)
  glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
  glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
  // get difference vector between both centers
  glm::vec2 difference = center - aabb_center;
  glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
  // now that we know the the clamped values, add this to AABB_center and we get the value of box closest to circle
  glm::vec2 closest = aabb_center + clamped;
  // now retrieve vector between center circle and closest point AABB and check if length < radius
  difference = closest - center;

  //printf("Col %f<%f\n", glm::length(difference), one.Radius);
  if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
  {
    // printf("YEEES !!!!!!\n");
    return std::make_tuple(true, VectorDirection(difference), difference);
  }
  else
  {
    return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
  }
}


Game::Game(unsigned int width, unsigned int height)
  : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
  memset(KeysProcessed, 0, sizeof(KeysProcessed));
  Level = 0;
  Lives = 3;
}

Game::~Game()
{
  delete Renderer;
  delete Ball;
  delete Text;
  delete Player;
  delete Particles;
  ma_engine_uninit(pAudioEngine);
  delete pAudioEngine;
}

void Game::Init()
{
  // load shaders
  Shader shader = ResourceManager::LoadShader("C:/pro/OpenGl/OpenGl/res/shader/sprite.vs", "C:/pro/OpenGl/OpenGl/res/shader/sprite.fs", nullptr, "sprite");
  if (shader.IsValid())
  {
    shader = ResourceManager::LoadShader("C:/pro/OpenGl/OpenGl/res/shader/particle.vs", "C:/pro/OpenGl/OpenGl/res/shader/particle.fs", nullptr, "particle");
    if (shader.IsValid())
    {
      //ResourceManager::LoadShader("C:/pro/OpenGl/OpenGl/text_2d.vs", "C:/pro/OpenGl/OpenGl/text_2d.fs", nullptr, "sprite");
      // configure shaders
      glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
      ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
      ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
      ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
      ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
      // load textures
      Texture2D tex = ResourceManager::LoadTexture("C:/pro/OpenGl/OpenGl/res/texture/awesomeface.png", true, "face");
      if (!tex.IsValid())
      {
        PostGlErrorMessage("Could not load awesomeface texture");
      }
      tex = ResourceManager::LoadTexture("C:/pro/OpenGl/OpenGl/res/texture/paddle.png", true, "paddle");
      if (!tex.IsValid())
      {
        PostGlErrorMessage("Could not load paddle texture");
      }
//      tex = ResourceManager::LoadTexture("C:/pro/OpenGl/OpenGl/res/texture/particle.png", true, "particle");
//      tex = ResourceManager::LoadTexture("C:/pro/OpenGl/OpenGl/res/texture/particle_red.png", true, "particle");
      tex = ResourceManager::LoadTexture("C:/pro/OpenGl/OpenGl/res/texture/particle_face.png", true, "particle");
      if (!tex.IsValid())
      {
        PostGlErrorMessage("Could not load particle texture");
      }
      // set render-specific controls
      Shader shader = ResourceManager::GetShader("sprite");
      Renderer = new SpriteRenderer(shader);
      Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
      if (!Particles->IsValid())
      {
        PostGlErrorMessage("Could not load ParticleGenerator");
      }

      // configure game objects
      glm::vec3 playerPos = glm::vec3(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y, 0.0f);
      Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
      glm::vec3 ballPos = playerPos + glm::vec3(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f, 0.0f);
      Ball = new BofBallObject2d(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
      Text = new TextRenderer(this->Width, this->Height);
      Text->Load("C:/pro/OpenGl/OpenGl/res/font/Antonio-Regular.ttf", 24);

      //Audio high level
      ma_result result;
      ma_engine_config engineConfig;

      //      pAudioEngine = (ma_engine*)malloc(sizeof(ma_engine));
      pAudioEngine = new ma_engine;
      if (pAudioEngine)
      {
        engineConfig = ma_engine_config_init();
        //engineConfig.pResourceManager = &myCustomResourceManager;   // <-- Initialized as some earlier stage.

        result = ma_engine_init(&engineConfig, pAudioEngine);
        if (result == MA_SUCCESS)
        {
          result = ma_sound_init_from_file(pAudioEngine, "C:/pro/OpenGl/OpenGl/res/audio/breakout.mp3", 0, NULL, NULL, &music);
          if (result == MA_SUCCESS)
          {
            result = ma_sound_init_from_file(pAudioEngine, "C:/pro/OpenGl/OpenGl/res/audio/bleep.mp3", 0, NULL, NULL, &bleep);
            if (result == MA_SUCCESS)
            {
              result = ma_sound_init_from_file(pAudioEngine, "C:/pro/OpenGl/OpenGl/res/audio/solid.wav", 0, NULL, NULL, &solid);
              if (result == MA_SUCCESS)
              {
                ma_sound_start(&music);
              }
            }
          }
        }
      }
    }
    else
    {
      PostGlErrorMessage("Could not load particle shader");
    }
  }
  else
  {
    PostGlErrorMessage("Could not load sprite shader");
  }
}
void Game::DoCollisions()
{
  // and finally check collisions for player pad (unless stuck)

  Collision result = CheckCollision(*Ball, *Player);
  if (!Ball->Stuck && std::get<0>(result))
  {
    // check where it hit the board, and change velocity based on where it hit the board
    float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
    float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
    float percentage = distance / (Player->Size.x / 2.0f);
    // then move accordingly
    float strength = 2.0f;
    glm::vec2 oldVelocity = Ball->Velocity;
    Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    //Ball->Velocity.y = -Ball->Velocity.y;
    Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
    // fix sticky paddle
    Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

    // if Sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
    Ball->Stuck = Ball->Sticky;

    ma_sound_start(&bleep);
    //SoundEngine->play2D(FileSystem::getPath("resources/audio/bleep.wav").c_str(), false);
  }
}

void Game::Update(float dt)
{
  bool Bounce_B;
  //Press space Ball->Stuck = false;
  Ball->Move(dt, this->Width, this->Height, Bounce_B);
  if (Bounce_B)
  {
    ma_sound_start(&solid);
  }
  this->DoCollisions();
  // update particles
  Particles->Update(dt, *Ball, 2, glm::vec3(Ball->Radius / 2.0f, Ball->Radius / 2.0f, 0.0f));
}

void Game::ProcessInput(float dt)
{
  // if (this->State == GAME_ACTIVE)
  {
    float velocity = PLAYER_VELOCITY * dt;
    // move playerboard
    if (this->Keys[GLFW_KEY_LEFT])
    {
      if (Player->Position.x >= 0.0f)
      {
        Player->Position.x -= velocity;
        if (Ball->Stuck)
          Ball->Position.x -= velocity;
      }
    }
    if (this->Keys[GLFW_KEY_RIGHT])
    {
      if (Player->Position.x <= this->Width - Player->Size.x)
      {
        Player->Position.x += velocity;
        if (Ball->Stuck)
          Ball->Position.x += velocity;
      }
    }
    if (this->Keys[GLFW_KEY_SPACE])
      Ball->Stuck = false;
  }
}

void Game::Render()
{
  static float S_Alpha_f = 0.0f, S_AlphaInc_f = 0.005f;
  Texture2D texture = ResourceManager::GetTexture("face");
  Renderer->DrawSprite(texture, glm::vec3(200.0f, 200.0f, 0.0f), glm::vec3(500.0f, 500.0f, 0.0f), 45.0f, glm::vec4(1.0f, 1.0f, 1.0f, S_Alpha_f));
  S_Alpha_f += S_AlphaInc_f;
  if (S_AlphaInc_f > 0)
  {
    if (S_Alpha_f > 0.95f)
    {
      S_AlphaInc_f = -S_AlphaInc_f;
    }
  }
  else
  {
    if (S_Alpha_f < 0.05f)
    {
      S_AlphaInc_f = -S_AlphaInc_f;
    }
  }


  Renderer->DrawSprite(texture, glm::vec3(0.0f, 0.0f, 0.8f), glm::vec3(100.0f, 100.0f, 0.0f), -90.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  Renderer->DrawSprite(texture, glm::vec3(50.0f, 0.0f, 0.6f), glm::vec3(100.0f, 100.0f, 0.0f), 90.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  Renderer->DrawSprite(texture, glm::vec3(100.0f, 0.0f, 0.4f), glm::vec3(100.0f, 100.0f, 0.0f), 180.0f, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));

  Text->RenderText("Another string !", glm::vec3(100.0f, 115.0f, -0.1f), 2.0f, glm::vec3(0.0f, 0.4f, 0.2f));
  Renderer->DrawSprite(texture, glm::vec3(100.0f, 100.0f, 0.0f), glm::vec3(100.0f, 100.0f, 0.0f), 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));

  if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
  {
    Text->RenderText("Hello World !", glm::vec3(5.0f, 5.0f, 0.0f), 1.0f, glm::vec3(1.0f, 0, 0));
  }
  Ball->Draw(*Renderer);
  // draw player
  Player->Draw(*Renderer);
  // draw particles	
  Particles->Draw();
}
END_BOF2D_NAMESPACE()