/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bof2d class
 *
 * Name:        ut_enum.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include "gtestrunner.h"

#include <bof2d/engine/bof2d_game_engine.h>
#include <bof2d/engine/bof2d_res_mgr.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
BOF2D::Game Breakout(SCR_WIDTH, SCR_HEIGHT);

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (key >= 0 && key < 1024)
  {
    if (action == GLFW_PRESS)
      Breakout.Keys[key] = true;
    else if (action == GLFW_RELEASE)
      Breakout.Keys[key] = false;
  }
}
void APIENTRY glDebugOutput(GLenum source,
  GLenum type,
  unsigned int id,
  GLenum severity,
  GLsizei length,
  const char* message,
  const void* userParam)
{
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source)
  {
  case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
  case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
  case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
  } std::cout << std::endl;

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
  case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
  case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
  case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
  case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
  case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
  case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
  } std::cout << std::endl;

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
  case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
  case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
  case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
  } std::cout << std::endl;
  std::cout << std::endl;
}

TEST(Bof2d_GameEngine_Test, DISABLED_Loop)
{
  /* Initialize the library */
  if (glfwInit())
  {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Smile for LvGl !", NULL, NULL);
    if (window)
    {
      glfwMakeContextCurrent(window);
      glfwSetKeyCallback(window, key_callback);
      glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

      glfwMakeContextCurrent(window);

      // glad: load all OpenGL function pointers
     // ---------------------------------------
      if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      {

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        printf("flg %08X\n", flags);
        //extern void *wglGetProcAddress(const char *lpszProc);
        PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)glfwGetProcAddress("glDebugMessageCallback");
        PFNGLDEBUGMESSAGECONTROLPROC  glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)glfwGetProcAddress("glDebugMessageControl");
        PFNGLDEBUGMESSAGEINSERTPROC  glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)glfwGetProcAddress("glDebugMessageInsert");

        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
          // initialize debug output 
          glEnable(GL_DEBUG_OUTPUT);
          glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
          glDebugMessageCallback(glDebugOutput, nullptr);
          glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }

#if defined(TEST_DEBUG)
        glGenTextures(-5, nullptr);
        glCheckError();
        //Or this one: std::cout << glGetError() << std::endl; // returns 1281 (invalid value)
        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_MEDIUM, -1, "bha error message here");
#endif

        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        Breakout.Init();

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
          // calculate delta time
          // --------------------
          float currentFrame = (float)glfwGetTime();
          deltaTime = currentFrame - lastFrame;
          lastFrame = currentFrame;
          glfwPollEvents();

          Breakout.ProcessInput(deltaTime);

          Breakout.Update(deltaTime);

          glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          Breakout.Render();

          glfwSwapBuffers(window);
        }
        BOF2D::ResourceManager::Clear();
      }
    }
    glfwTerminate();
  }
}