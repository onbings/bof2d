/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof module interface to convert media
 *
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * History:
 *
 * V 1.00  Nov 13 2022  BHA : Initial release
 */
#pragma once
#include <bofstd/bofsystem.h>

#include <bof2d/bof2d.h>

#define _USE_MATH_DEFINES
#include <math.h>

BEGIN_BOF2D_NAMESPACE()
//Video
BOF2D_EXPORT void Bof_YuvToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);
// JPEG YUV to RGB reference
BOF2D_EXPORT void Bof_YuvjToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);
// BT.709 YUV to RGB reference
// See also http://www.equasys.de/colorconversion.html
BOF2D_EXPORT void Bof_YuvhToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);
// BT.2020 YUV to RGB reference
BOF2D_EXPORT void Bof_YuvRec2020ToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);

BOF2D_EXPORT BOFERR Bof_UyvyToBgra(uint32_t _Width_U32, int _Height_i, uint32_t _UyvyStride_U32, const uint8_t *_pUyvy_U8, uint32_t _BrgaStride_U32, BOF_RECT *_pCrop_X, BOF_ROTATION _Rotation_E, uint8_t *_pBgra_U8);
BOF2D_EXPORT BOFERR Bof_BgraToUyvy(uint32_t _Width_U32, int _Height_i, uint32_t _BrgaStride_U32, const uint8_t *_pBgra_U8, uint32_t _UyvyStride_U32, BOF_RECT *_pCrop_X, BOF_ROTATION _Rotation_E, uint8_t *_pUyvy_U8);

//Audio
BOF2D_EXPORT BOFERR Bof_24sleTo32sle(const BOF::BOF_BUFFER &_rSrcBuffer_X, BOF::BOF_BUFFER &_rDstBuffer_X);
BOF2D_EXPORT BOFERR Bof_32sleTo24sle(const BOF::BOF_BUFFER &_rSrcBuffer_X, BOF::BOF_BUFFER &_rDstBuffer_X);


class BOF2D_EXPORT Bof_AudioSignalGeneratorSinus
{
public:
  Bof_AudioSignalGeneratorSinus(float _AmplitudeMax_f, float _FrequencyInHz_f, uint32_t _SampleRateInHz_U32);
  Bof_AudioSignalGeneratorSinus();
  float Next();
  void Reset();
  void Reset(float _FrequencyInHz_f, uint32_t _SampleRateInHz_U32);
  float Frequency() const;
  float AmplitudeMax() const;
  void AmplitudeMax(float _AmplitudeMax_f);
  uint32_t SampleRate() const;

private:
  constexpr static float mTwoPi_f = 2.0f * M_PI;
  float                  mSinusIncrement_f = 1.f;
  float                  mFrequencyInHz_f = 440; 
  int                    mSampleRateInHz_U32 = 48000;
  float                  mAmplitudeMax_f = 1.0f;
  int32_t                mSampleNumber_U32 = 0;
};

END_BOF2D_NAMESPACE()
