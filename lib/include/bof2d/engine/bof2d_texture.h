/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof2d texture object
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

#include <glad/glad.h>

#include <bof2d/bof2d.h>

BEGIN_BOF2D_NAMESPACE()
// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class BOF2D_EXPORT Texture2D
{
public:
  // holds the ID of the texture object, used for all texture operations to reference to this particlar texture
  unsigned int ID;
  // texture image dimensions
  unsigned int Width, Height; // width and height of loaded image in pixels
  // texture Format
  unsigned int Internal_Format; // format of texture object
  unsigned int Image_Format; // format of loaded image
  // texture configuration
  unsigned int Wrap_S; // wrapping mode on S axis
  unsigned int Wrap_T; // wrapping mode on T axis
  unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
  unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels
  // constructor (sets default texture modes)
  Texture2D();
  bool IsValid() const;
  // generates texture from image data
  void Generate(unsigned int width, unsigned int height, unsigned char *data, int nrChannels);
  // binds the texture as the current active GL_TEXTURE_2D texture object
  void Bind() const;
};
END_BOF2D_NAMESPACE()