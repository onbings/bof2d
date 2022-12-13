/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d shader object
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

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <bof2d/bof2d.h>

BEGIN_BOF2D_NAMESPACE()
// General purpose shader object. Compiles from file, generates
// compile/link-time error messages and hosts several utility 
// functions for easy management.
class BOF2D_EXPORT Shader
{
public:
  // state
//  unsigned int ID;
  uint32_t ID;
  // constructor
  Shader();
  // sets the current shader as active
  Shader& Use();
  bool IsValid() const;
  // compiles the shader from given source code
  //Done with the resource manager void    LoadAndCompile(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
  void    Compile(const char* vertexCode, const char* fragmentCode, const char* geometryCode = nullptr); 
  // utility functions
  void    SetBoolean(const char* name, bool value, bool useShader = false);
  void    SetInteger(const char* name, int value, bool useShader = false);
  void    SetFloat(const char* name, float value, bool useShader = false);
  void    SetVector2f(const char* name, float x, float y, bool useShader = false);
  void    SetVector2f(const char* name, const glm::vec2& value, bool useShader = false);
  void    SetVector3f(const char* name, float x, float y, float z, bool useShader = false);
  void    SetVector3f(const char* name, const glm::vec3& value, bool useShader = false);
  void    SetVector4f(const char* name, float x, float y, float z, float w, bool useShader = false);
  void    SetVector4f(const char* name, const glm::vec4& value, bool useShader = false);
  void    SetMatrix2(const char* name, const glm::mat2& matrix, bool useShader = false);
  void    SetMatrix3(const char* name, const glm::mat3& matrix, bool useShader = false);
  void    SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader = false);
private:
  // checks if compilation or linking failed and if so, print the error logs
  void    CheckCompileErrors(unsigned int object, std::string type);
};

END_BOF2D_NAMESPACE()