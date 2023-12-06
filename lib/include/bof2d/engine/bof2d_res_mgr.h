/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d resource manager object
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
#include <string>

#include <glad/glad.h>

#include <bof2d/engine/bof2d_texture.h>
#include <bof2d/engine/bof2d_shader.h>

BEGIN_BOF2D_NAMESPACE()
// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class BOF2D_EXPORT ResourceManager
{
public:
  // resource storage
  static std::map<std::string, Shader>    Shaders;
  static std::map<std::string, Texture2D> Textures;
  // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
  static Shader    LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name);
  // retrieves a stored sader
  static Shader    GetShader(std::string name);
  // loads (and generates) a texture from file
  static Texture2D LoadTexture(const char *file, bool alpha, std::string name);
  // retrieves a stored texture
  static Texture2D GetTexture(std::string name);
  // properly de-allocates all loaded resources
  static void      Clear();
private:
  // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
  ResourceManager() { }
  // loads and generates a shader from file
  static Shader    loadShaderFromFile(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);
  // loads a single texture from file
  static Texture2D loadTextureFromFile(const char *file, bool alpha);
};
END_BOF2D_NAMESPACE()