/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d sprite renderer object
 *
 * Name:        bof2d.h
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
#include <glm/gtc/matrix_transform.hpp>

#include <bof2d/engine/bof2d_texture.h>
#include <bof2d/engine/bof2d_shader.h>

BEGIN_BOF2D_NAMESPACE()
class BOF2D_EXPORT SpriteRenderer
{
public:
  // Constructor (inits shaders/shapes)
  SpriteRenderer(Shader &shader);
  // Destructor
  ~SpriteRenderer();
  // Renders a defined quad textured with given sprite
  void DrawSprite(Texture2D &texture, glm::vec3 position, glm::vec3 size, float rotate, glm::vec4 color);
private:
  // Render state
  Shader       shader;
  unsigned int quadVAO;
  // Initializes and configures the quad's buffer and vertex attributes
  void initRenderData();
};

END_BOF2D_NAMESPACE()