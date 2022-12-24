/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d opengl object
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
/*
glfw: https://www.youtube.com/watch?v=HzFatL3WT6g -> https://www.glfw.org/download
glad//KHR/khrplatform.h: https://glad.dav1d.de/
  In Language leave C/C++.
  Below API, in gl entry, select the latest version (today is 4.6).
  In Specification leave OpenGL.
  In Profile select Core.
  Scroll down the page and click GENERATE.
  In "Glad" window (see picture above), click "glad.zip".
  In downloading window you have two zip folders: include and src.
  add C:\pro\third\glad\src{glad.c to the project

glm: https://github.com/g-truc/glm
freetype: https://www.youtube.com/watch?v=qW_8Dyq2asc -> https://sourceforge.net/projects/freetype/
  open C:\pro\third\freetype\builds\windows\vc2010\freetype.sln and compile the 4 configurations
image reader/writer: https://github.com/nothings/stb
->Not used Codehead’s Bitmap Font Generator: http://www.codehead.co.uk/cbfg/

LearnOpenGL repo: https://github.com/JoeyDeVries/LearnOpenGL
shader: http://sdz.tdct.org/sdz/les-shaders-en-glsl.html https://www.khronos.org/opengl/wiki/Vertex_Shader
opengl-shader: https://www.khronos.org/opengl/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)
--->https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites

glview: https://realtech-vr.com/home/glview
*/

#include <iostream>
#include <map>
#include <string>

#include <glad/glad.h>
#if defined(ANDROID)
#else
#include <GLFW/glfw3.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <bof2d/engine/bof2d_res_mgr.h>
#include <bof2d/engine/bof2d_game_engine.h>

BEGIN_BOF2D_NAMESPACE()

GLenum glCheckError_(const char* file, int line)
{
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR)
  {
    std::string error;
    switch (errorCode)
    {
    case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
    case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
    case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
    case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
    case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
    }
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
  }

  return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 


//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

// settings


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  unsigned int TextureID; // ID handle of the glyph texture
  glm::ivec2   Size;      // Size of glyph
  glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
  unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int VAO, VBO;


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
#if defined(ANDROID)
#else
void processInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

bool PostGlErrorMessage(const char* _pError_c)
{
  bool Rts_B = false;
  PFNGLDEBUGMESSAGEINSERTPROC  glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)glfwGetProcAddress("glDebugMessageInsert");

  if (glDebugMessageInsert)
  {
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_MEDIUM, -1, _pError_c);
    Rts_B = true;
  }
  return Rts_B;
}
#endif

END_BOF2D_NAMESPACE()