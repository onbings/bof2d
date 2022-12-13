/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d particle generator object
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

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <bof2d/engine/bof2d_texture.h>
#include <bof2d/engine/bof2d_shader.h>
#include <bof2d/engine/bof2d_game_obj.h>

BEGIN_BOF2D_NAMESPACE()
// Represents a single particle and its state
struct Particle {
  glm::vec3 Position, Velocity;
  glm::vec4 Color;
  float     Life;

  Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class BOF2D_EXPORT ParticleGenerator
{
public:
  // constructor
  ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
  // update all particles
  void Update(float dt, GameObject& object, unsigned int newParticles, glm::vec3 offset); // = glm::vec2(0.0f, 0.0f));
  // render all particles
  void Draw();
  bool IsValid();
private:
  // state
  std::vector<Particle> particles;
  unsigned int amount;
  // render state
  Shader shader;
  Texture2D texture;
  unsigned int VAO;
  // initializes buffer and vertex attributes
  void init();
  // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
  unsigned int firstUnusedParticle();
  // respawns particle
  void respawnParticle(Particle& particle, GameObject& object, glm::vec3 offset); // = glm::vec2(0.0f, 0.0f));
};

END_BOF2D_NAMESPACE()