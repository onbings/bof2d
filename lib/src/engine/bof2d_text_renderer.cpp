/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof2d text renderer object
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
#include <bof2d/engine/bof2d_text_renderer.h>
#include <bof2d/engine/bof2d_res_mgr.h>

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H


BEGIN_BOF2D_NAMESPACE()

TextRenderer::TextRenderer(unsigned int width, unsigned int height)
{
  // load and configure shader
  this->TextShader = ResourceManager::LoadShader("./res/shader/text_2d.vs", "./res/shader/text_2d.fs", nullptr, "text");
  if (TextShader.IsValid())
  {
    this->TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
    this->TextShader.SetInteger("text", 0);
    // configure VAO/VBO for texture quads
        /* Allocate and assign a Vertex Array Object to our handle */
    glGenVertexArrays(1, &vao);

    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray(vao);

    /* Allocate and assign two Vertex Buffer Objects to our handle */
    glGenBuffers(2, vbo);

    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    /* Copy the vertex data from diamond to our buffer */
    /* 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values */
    //glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), diamond, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);

    /* Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex */
//      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    /* Enable attribute index 0 as being used */
    glEnableVertexAttribArray(0);

    /* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    /* Copy the color data from colors to our buffer */
    /* 12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values */
//      glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, NULL, GL_DYNAMIC_DRAW);

    /* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /* Enable attribute index 1 as being used */
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); //??
    glBindVertexArray(0); //??
  }
}

TextRenderer::~TextRenderer()
{
  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(1, &vao);
}
void TextRenderer::Load(std::string font, unsigned int fontSize)
{
  // first clear the previously loaded Characters
  this->Characters.clear();
  // then initialize and load the FreeType library
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
  // load font as face
  FT_Face face;
  if (FT_New_Face(ft, font.c_str(), 0, &face))
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
  // set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, fontSize);
  // disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // then for the first 128 ASCII characters, pre-load/compile their characters and store them
  for (GLubyte c = 0; c < 128; c++) // lol see what I did there 
  {
    // load character glyph 
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
      continue;
    }
    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // now store character for later use
    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        (int)face->glyph->advance.x
    };
    Characters.insert(std::pair<char, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  // destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, glm::vec3 position, float scale, glm::vec3 color)
{
  // activate corresponding render state	
  this->TextShader.Use();
  this->TextShader.SetVector3f("textColor", color);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(this->vao);
  
  // iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++)
  {
    Character ch = Characters[*c];

    float xpos = position.x + ch.Bearing.x * scale;
    float ypos = position.y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;
    float pos[6][3] = {
        { xpos,     ypos + h, position.z},
        { xpos + w, ypos,     position.z},
        { xpos,     ypos,     position.z},

        { xpos,     ypos + h, position.z},
        { xpos + w, ypos + h, position.z},
        { xpos + w, ypos,     position.z}
    };
    float tex[6][2] = {
        { 0.0f, 1.0f },
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },

        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 1.0f, 0.0f }
    };
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pos), pos); // be sure to use glBufferSubData and not glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tex), tex); // be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph
    position.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

}

END_BOF2D_NAMESPACE()