/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof2d shader object
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
#include <bof2d/engine/bof2d_shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

BEGIN_BOF2D_NAMESPACE()

Shader::Shader() : ID(0)
{

}
bool Shader::IsValid() const
{
  return (ID != 0);   //No always return false ??? && (glIsShader(ID)));
}
Shader& Shader::Use()
{
  glUseProgram(this->ID);
  return *this;
}
/*
void Shader::LoadAndCompile(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
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

    Compile(vShaderCode, fShaderCode, gShaderCode);
  }
  catch (std::ifstream::failure& e)
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
  }
}
*/
void Shader::Compile(const char* vertexCode, const char* fragmentCode, const char* geometryCode)
{
  uint32_t vertex, fragment, geometry;
  // vertex shader
  if (vertexCode != nullptr)
  {
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");
  }
  // fragment Shader
  if (fragmentCode != nullptr)
  {
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");
  }
  // if geometry shader is given, compile geometry shader
  if (geometryCode != nullptr)
  {
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &geometryCode, NULL);
    glCompileShader(geometry);
    CheckCompileErrors(geometry, "GEOMETRY");
  }
  // shader Program
  ID = glCreateProgram();
  if (vertexCode != nullptr)
  {
    glAttachShader(ID, vertex);
  }
  if (fragmentCode != nullptr)
  {
    glAttachShader(ID, fragment);
  }
  if (geometryCode != nullptr)
  {
    glAttachShader(ID, geometry);
  }
  glLinkProgram(ID);
  CheckCompileErrors(ID, "PROGRAM");
  // delete the shaders as they're linked into our program now and no longer necessery
  if (vertexCode != nullptr)
  {
    glDeleteShader(vertex);
  }
  if (fragmentCode != nullptr)
  {
    glDeleteShader(fragment);
  }
  if (geometryCode != nullptr)
  {
    glDeleteShader(geometry);
  }
}

void Shader::SetBoolean(const char* name, bool value, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform1i(glGetUniformLocation(this->ID, name), (int)value);
}
void Shader::SetInteger(const char* name, int value, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetFloat(const char* name, float value, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void Shader::SetVector2f(const char* name, float x, float y, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVector3f(const char* name, float x, float y, float z, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
{
  if (useShader)
    this->Use();
  glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix2(const char* name, const glm::mat2& matrix, bool useShader)
{
  if (useShader)
    this->Use();
  glUniformMatrix2fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}
void Shader::SetMatrix3(const char* name, const glm::mat3& matrix, bool useShader)
{
  if (useShader)
    this->Use();
  glUniformMatrix3fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}
void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
{
  if (useShader)
    this->Use();
  glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}


void Shader::CheckCompileErrors(unsigned int object, std::string type)
{
  int success;
  char infoLog[1024];
  if (type != "PROGRAM")
  {
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(object, 1024, NULL, infoLog);
      std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
        << infoLog << "\n -- --------------------------------------------------- -- "
        << std::endl;
    }
  }
  else
  {
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(object, 1024, NULL, infoLog);
      std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
        << infoLog << "\n -- --------------------------------------------------- -- "
        << std::endl;
    }
  }
}

END_BOF2D_NAMESPACE()