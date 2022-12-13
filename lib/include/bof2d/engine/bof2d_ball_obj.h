/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d ball object
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

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <bof2d/engine/bof2d_game_obj.h>
#include <bof2d/engine/bof2d_texture.h>

BEGIN_BOF2D_NAMESPACE()

class BOF2D_EXPORT BofBallObject2d : public GameObject
{
public:
  float   Radius;
  bool    Stuck;
  bool    Sticky, PassThrough;

  BofBallObject2d();
  BofBallObject2d(glm::vec3 pos, float radius, glm::vec3 velocity, Texture2D sprite);
  // moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
  glm::vec2 Move(float dt, unsigned int window_width, unsigned int window_height, bool& _rBounce_B);
  // resets the ball to original state with given position and velocity
  void      Reset(glm::vec3 position, glm::vec3 velocity);
};

END_BOF2D_NAMESPACE()
