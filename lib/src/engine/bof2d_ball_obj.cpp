/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof2d ball object
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
#include <Bof2d/engine/bof2d_ball_obj.h>

BEGIN_BOF2D_NAMESPACE()

BofBallObject2d::BofBallObject2d() : GameObject(), Radius(12.5f), Stuck(true), Sticky(false), PassThrough(false) { }

BofBallObject2d::BofBallObject2d(glm::vec3 pos, float radius, glm::vec3 velocity, Texture2D sprite)
  : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec4(1.0f), velocity), Radius(radius), Stuck(true), Sticky(false), PassThrough(false) { }

glm::vec2 BofBallObject2d::Move(float dt, unsigned int window_width, unsigned int window_height, bool &_rBounce_B)
{
  _rBounce_B = false;
  // if not stuck to player board
  if (!this->Stuck)
  {
    // move the ball
    this->Position += this->Velocity * dt;
    // then check if outside window bounds and if so, reverse velocity and restore at correct position
    if (this->Position.x <= 0.0f)
    {
      this->Velocity.x = -this->Velocity.x;
      this->Position.x = 0.0f;
      _rBounce_B = true;
    }
    else if (this->Position.x + this->Size.x >= window_width)
    {
      this->Velocity.x = -this->Velocity.x;
      this->Position.x = window_width - this->Size.x;
      _rBounce_B = true;
    }
    if (this->Position.y <= 0.0f)
    {
      this->Velocity.y = -this->Velocity.y;
      this->Position.y = 0.0f;
      _rBounce_B = true;
    }
    else if (this->Position.y + this->Size.y >= window_height)
    {
      this->Velocity.y = -this->Velocity.y;
      this->Position.y = window_height - this->Size.y;
      _rBounce_B = true;
    }

  }
  return this->Position;
}

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void BofBallObject2d::Reset(glm::vec3 position, glm::vec3 velocity)
{
  this->Position = position;
  this->Velocity = velocity;
  this->Stuck = true;
  this->Sticky = false;
  this->PassThrough = false;

}

END_BOF2D_NAMESPACE()