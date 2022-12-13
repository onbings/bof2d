/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d game object
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

#include <bof2d/engine/bof2d_texture.h>
#include <bof2d/engine/bof2d_sprite_renderer.h>


BEGIN_BOF2D_NAMESPACE()

class BOF2D_EXPORT GameObject
{
public:
  // object state
  glm::vec3   Position, Velocity;
  glm::vec2 Size;
  glm::vec4   Color;
  float       Rotation;
  bool        IsSolid;
  bool        Destroyed;
  // render state
  Texture2D   Sprite;
  // constructor(s)
  GameObject();
  GameObject(glm::vec3 pos, glm::vec2 size, Texture2D sprite, glm::vec4 color, glm::vec3 velocity);
  // draw sprite
  virtual void Draw(SpriteRenderer& renderer);
  bool IsValid();
};
END_BOF2D_NAMESPACE()
