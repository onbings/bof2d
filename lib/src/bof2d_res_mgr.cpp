/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof2d resource manager object
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
#include <bof2d/bof2d_res_mgr.h>

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BEGIN_BOF2D_NAMESPACE()

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;


Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
  Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
  return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
  return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
  Textures[name] = loadTextureFromFile(file, alpha);
  return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
  return Textures[name];
}

void ResourceManager::Clear()
{
  // (properly) delete all shaders	
  for (auto iter : Shaders)
    glDeleteProgram(iter.second.ID);
  // (properly) delete all textures
  for (auto iter : Textures)
    glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
//const char *vShaderFile, const char *fShaderFile, const char *gShaderFile)
{
  Shader shader;
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vertexCode, fragmentCode, geometryCode;
  std::ifstream vShaderFile, fShaderFile, gShaderFile;
  std::stringstream vShaderStream, fShaderStream, gShaderStream;
  const char* vShaderCode = nullptr, * fShaderCode = nullptr, * gShaderCode = nullptr;

  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try
  {
    // open files
    if (vertexPath != nullptr)
    {
      vShaderFile.open(vertexPath);
      vShaderStream << vShaderFile.rdbuf();
      vShaderFile.close();
      vertexCode = vShaderStream.str();
      vShaderCode = vertexCode.c_str();
    }

    if (fragmentPath != nullptr)
    {
      fShaderFile.open(fragmentPath);
      fShaderStream << fShaderFile.rdbuf();
      fShaderFile.close();
      fragmentCode = fShaderStream.str();
      fShaderCode = fragmentCode.c_str();
    }

    if (geometryPath != nullptr)
    {
      gShaderFile.open(geometryPath);
      gShaderStream << gShaderFile.rdbuf();
      gShaderFile.close();
      geometryCode = gShaderStream.str();
      gShaderCode = geometryCode.c_str();
    }

    // 2. now create shader object from source code
    shader.Compile(vShaderCode, fShaderCode, gShaderCode);
  }
  catch (std::ifstream::failure& e)
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
  }
  return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
  // create texture object
  Texture2D texture;
  if (alpha)
  {
    texture.Internal_Format = GL_RGBA;
    texture.Image_Format = GL_RGBA;
  }
  // load image
  int width, height, nrChannels;
  unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
  if (data)
  {
    // now generate texture
    texture.Generate(width, height, data, nrChannels);
    // and finally free image data
    stbi_image_free(data);
  }
  return texture;
}

END_BOF2D_NAMESPACE()