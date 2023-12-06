/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof2d texture object
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

#include <iostream>

BEGIN_BOF2D_NAMESPACE()

Texture2D::Texture2D()
  : Width(0), Height(0), Internal_Format(GL_RGBA), Image_Format(GL_RGBA), Wrap_S(GL_CLAMP_TO_EDGE), Wrap_T(GL_CLAMP_TO_EDGE), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
  //  : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
  glGenTextures(1, &this->ID);
}

bool Texture2D::IsValid() const
{
  return ((Width) && (Height) && (glIsTexture(ID)));
  //  return glIsTexture(ID);
}
void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char *data, int nrChannels)
{
  this->Width = width;
  this->Height = height;
  // create Texture
  glBindTexture(GL_TEXTURE_2D, this->ID);
  //  glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
  if (nrChannels == 4)
  {
    Internal_Format = GL_RGBA;
    Image_Format = GL_RGBA;
    Wrap_S = GL_CLAMP_TO_EDGE;
  }
  else
  {
    Internal_Format = GL_RGBA;
    Image_Format = GL_RGB;
    Wrap_S = GL_CLAMP_TO_EDGE;  // GL_REPEAT;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
  // set Texture wrap and filter modes
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
  // unbind texture
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
  glBindTexture(GL_TEXTURE_2D, this->ID);
}

END_BOF2D_NAMESPACE()