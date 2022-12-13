/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof2d particle generator object
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
#include <bof2d/engine/bof2d_particle_gen.h>

BEGIN_BOF2D_NAMESPACE()

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
  : shader(shader), texture(texture), amount(amount)
{
  this->init();
}
bool ParticleGenerator::IsValid()
{
  return texture.IsValid();
}
void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec3 offset)
{
  // add new particles 
  
  for (unsigned int i = 0; i < newParticles; ++i)
  {
    int unusedParticle = this->firstUnusedParticle();
    this->respawnParticle(this->particles[unusedParticle], object, offset);
  }
  
  // update all particles
  for (unsigned int i = 0; i < this->amount; ++i)
  {
    Particle& p = this->particles[i];
    p.Life -= dt; // reduce life
    if (p.Life > 0.0f)
    {	// particle is alive, thus update
      p.Position -= p.Velocity * dt;
      p.Color.a -= dt;  // *2.5f;
    }
  }
}

// render all particles
void ParticleGenerator::Draw()
{
  // use additive blending to give it a 'glow' effect
// save off current state of blend enabled
  GLboolean blendEnabled;
  glGetBooleanv(GL_BLEND, &blendEnabled);

  // save off current state of src / dst blend functions
  GLint blendSrc;
  GLint blendDst;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  this->shader.Use();
  for (Particle particle : this->particles)
  {
    if (particle.Life > 0.0f)
    {
      this->shader.SetVector3f("offset", particle.Position);
      this->shader.SetVector4f("color", particle.Color);
      this->texture.Bind();
      glBindVertexArray(this->VAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
    }
  }
  // don't forget to reset to default blending mode
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// restore saved state of blend enabled and blend functions
  if (blendEnabled) {
    glEnable(GL_BLEND);
  }
  else {
    glDisable(GL_BLEND);
  }

  glBlendFunc(blendSrc, blendDst);
}

void ParticleGenerator::init()
{
  // set up mesh and attribute properties
  unsigned int VBO;
  float particle_quad[] = {
      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f
  };
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(this->VAO);
  // fill mesh buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
  // set mesh attributes
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glBindVertexArray(0);

  // create this->amount default particle instances
  for (unsigned int i = 0; i < this->amount; ++i)
    this->particles.push_back(Particle());
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
  // first search from last used particle, this will usually return almost instantly
  for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
    if (this->particles[i].Life <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }
  // otherwise, do a linear search
  for (unsigned int i = 0; i < lastUsedParticle; ++i) {
    if (this->particles[i].Life <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }
  // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
  lastUsedParticle = 0;
  return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec3 offset)
{
  float random = ((rand() % 100) - 50) / 10.0f;
  float rColor = 0.5f + ((rand() % 100) / 100.0f);
  particle.Position.x = object.Position.x + random + offset.x;
  particle.Position.y = object.Position.y + random + offset.y;
  particle.Position.z = object.Position.z + offset.z;
  //  particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
  particle.Color = glm::vec4(1, 1, 1, 1.0f);
  //particle.Position = object.Position + offset;
  particle.Life = 1.0f;
  particle.Velocity = object.Velocity * 0.1f;
}

END_BOF2D_NAMESPACE()