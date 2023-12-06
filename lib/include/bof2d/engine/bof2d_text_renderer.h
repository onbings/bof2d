/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d text renderer object
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

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <bof2d/engine/bof2d_texture.h>
#include <bof2d/engine/bof2d_shader.h>

BEGIN_BOF2D_NAMESPACE()
/// Holds all state information relevant to a character as loaded using FreeType
struct BOF2D_EXPORT Character {
  unsigned int TextureID; // ID handle of the glyph texture
  glm::ivec2   Size;      // size of glyph
  glm::ivec2   Bearing;   // offset from baseline to left/top of glyph
  /* unsigned*/ int Advance;   // horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class BOF2D_EXPORT TextRenderer
{
public:
  // constructor
  TextRenderer(unsigned int width, unsigned int height);
  ~TextRenderer();
  // pre-compiles a list of characters from the given font
  void Load(std::string font, unsigned int fontSize);
  // renders a string of text using the precompiled list of characters
  void RenderText(std::string text, glm::vec3 position, float scale, glm::vec3 color);
private:
  // holds a list of pre-compiled Characters
  std::map<char, Character> Characters;
  // shader used for text rendering
  Shader TextShader;

  // render state
  GLuint vao, vbo[2]; /* Create handles for our Vertex Array Object and two Vertex Buffer Objects */
};

END_BOF2D_NAMESPACE()