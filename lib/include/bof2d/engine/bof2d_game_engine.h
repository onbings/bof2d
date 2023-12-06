/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d game engine object
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
#pragma once

#include <vector>
#include <tuple>

#include <glad/glad.h>
#if defined(ANDROID)
#else
#include <GLFW/glfw3.h>
#endif
#include <glm/glm.hpp>

#include <bof2d/bof2d.h>

BEGIN_BOF2D_NAMESPACE()

bool PostGlErrorMessage(const char *_pError_c);

// Represents the current state of the game
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

// Initial size of the player paddle
const glm::vec3 PLAYER_SIZE(100.0f, 20.0f, 0.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec3 INITIAL_BALL_VELOCITY(100.0f, -350.0f, 0.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class BOF2D_EXPORT Game
{
public:
  // game state
  GameState               State;
  bool                    Keys[1024];
  bool                    KeysProcessed[1024];
  unsigned int            Width, Height;
  ////std::vector<GameLevel>  Levels;
  ////std::vector<PowerUp>    PowerUps;
  unsigned int            Level;
  unsigned int            Lives;
  uint32_t mLoopCounter_U32 = 0;
  // constructor/destructor
  Game(unsigned int width, unsigned int height);
  ~Game();
  // initialize game state (load all shaders/textures/levels)
  void Init();
  // game loop
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();
  void DoCollisions();
  // reset
  void ResetLevel();
  void ResetPlayer();
  // powerups
  ////void SpawnPowerUps(GameObject &block);
  void UpdatePowerUps(float dt);
};


END_BOF2D_NAMESPACE()