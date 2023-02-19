/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof module interface to convert media
 *
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * History:
 *
 * V 1.00  Nov 13 2022  BHA : Initial release
 */
#include <bof2d/bof2d_convert.h>

#include <libyuv.h>

BEGIN_BOF2D_NAMESPACE()

// Pick a method for rounding.
#define ROUND(f) static_cast<int>(f + 0.5f)
static uint8_t RoundToByte(float f)
{
  int i = ROUND(f);
  return (i < 0) ? 0 : ((i > 255) ? 255 : static_cast<uint8_t>(i));
}
//From /home/bha/pingu/bofstd/third/libyuv/unit_test/color_test.cc
// BT.601 YUV to RGB reference

void Bof_YuvToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(v - 128) * -1.596f);
  *g = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(u - 128) * 0.391f - static_cast<float>(v - 128) * 0.813f);
  *b = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(u - 128) * -2.018f);
}

// JPEG YUV to RGB reference
void Bof_YuvjToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = RoundToByte(static_cast<float>(y) - static_cast<float>(v - 128) * -1.40200f);
  *g = RoundToByte(static_cast<float>(y) - static_cast<float>(u - 128) * 0.34414f - static_cast<float>(v - 128) * 0.71414f);
  *b = RoundToByte(static_cast<float>(y) - static_cast<float>(u - 128) * -1.77200f);
}

// BT.709 YUV to RGB reference
// See also http://www.equasys.de/colorconversion.html
void Bof_YuvhToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(v - 128) * -1.793f);
  *g = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(u - 128) * 0.213f - static_cast<float>(v - 128) * 0.533f);
  *b = RoundToByte(static_cast<float>(y - 16) * 1.164f - static_cast<float>(u - 128) * -2.112f);
}

// BT.2020 YUV to RGB reference
void Bof_YuvRec2020ToRgbReference(int y, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = RoundToByte(static_cast<float>(y - 16) * 1.164384f - static_cast<float>(v - 128) * -1.67867f);
  *g = RoundToByte(static_cast<float>(y - 16) * 1.164384f - static_cast<float>(u - 128) * 0.187326f - static_cast<float>(v - 128) * 0.65042f);
  *b = RoundToByte(static_cast<float>(y - 16) * 1.164384f - static_cast<float>(u - 128) * -2.14177f);
}

BOFERR Bof_UyvyToBgra(uint32_t _Width_U32, int _Height_i, uint32_t /*_UyvyStride_U32*/, const uint8_t *_pUyvy_U8, uint32_t _BrgaStride_U32, BOF_RECT *_pCrop_X, BOF_ROTATION _Rotation_E, uint8_t *_pBgra_U8)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

#if 1
  //125 times faster than #else part
  BOF_RECT r_X(0, 0, _Width_U32, _Height_i);
  int      DstStrideBgra_i, Sts_i;

  if ((_pCrop_X) && (_pCrop_X->IsInside(r_X)))
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  else
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    DstStrideBgra_i = _BrgaStride_U32;
    if (_Rotation_E != BOF_ROTATION::BOF_ROTATION_0)
    {
      if ((_Rotation_E != BOF_ROTATION::BOF_ROTATION_90) || (_Rotation_E != BOF_ROTATION::BOF_ROTATION_180))
      {
        DstStrideBgra_i = _pCrop_X ? _pCrop_X->Height() * 4 : _Height_i * 4;
      }
    }
    //  int Sts_i = libyuv::UYVYToARGB(_pYuYv_U8, _YuYvStride_U32, _pARgb_U8, _ARgbStride_U32, _Width_U32, _Height_i);

    if (Rts_E == BOF_ERR_NO_ERROR)
    {

      //  int Sts_i = libyuv::ConvertToARGB(_pYuYv_U8,0,_pARgb_U8,_ARgbStride_U32,0,0,_Width_U32, _Height_i,_Width_U32,_Height_i,libyuv::RotationMode::kRotate0,libyuv::FOURCC_UYVY);
      Rts_E = BOF_ERR_INIT;
      if (_pCrop_X)
      {
        Sts_i = libyuv::ConvertToARGB(_pUyvy_U8, 0, _pBgra_U8, DstStrideBgra_i, _pCrop_X->Left_S32, _pCrop_X->Top_S32, _Width_U32, _Height_i, _pCrop_X->Width(), _pCrop_X->Height(), static_cast<libyuv::RotationMode>(_Rotation_E), libyuv::FOURCC_UYVY);
      }
      else
      {
        Sts_i = libyuv::ConvertToARGB(_pUyvy_U8, 0, _pBgra_U8, DstStrideBgra_i, 0, 0, _Width_U32, _Height_i, _Width_U32, _Height_i, static_cast<libyuv::RotationMode>(_Rotation_E), libyuv::FOURCC_UYVY);
      }
      if (Sts_i == 0)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
#else
  uint32_t      x_U32, y_U32, Height_U32, IncARgb_U32, IncYuYv_U32;
  uint8_t       y_U8, u_U8, v_U8;
  const uint8_t *pStartOfYuYv_U8;

  Height_U32 = (_Height_i < 0) ? -_Height_i : _Height_i;
  if ((_Width_U32) && ((_Width_U32 & 3) == 0) && (Height_U32) && (_YuYvStride_U32 >= (_Width_U32 * 2)) && (_pYuYv_U8) && (_ARgbStride_U32 >= (_Width_U32 * 4)) && (_pARgb_U8))
  {
    Rts_E = BOF_ERR_NO_ERROR;
    IncYuYv_U32 = (_YuYvStride_U32 - (_Width_U32 * 2));
    IncARgb_U32 = (_ARgbStride_U32 - (_Width_U32 * 4));
    _pYuYv_U8 = (_Height_i < 0) ? _pYuYv_U8 + ((Height_U32 - 1) * _YuYvStride_U32) : _pYuYv_U8;
    for (y_U32 = 0; y_U32 < Height_U32; y_U32++)
    {
      pStartOfYuYv_U8 = _pYuYv_U8;
      for (x_U32 = 0; x_U32 < (_Width_U32 / 2); x_U32++)
      {
        u_U8 = _pYuYv_U8[0];
        y_U8 = _pYuYv_U8[1];
        v_U8 = _pYuYv_U8[2];
        Bof_YuvToRgbReference(y_U8, u_U8, v_U8, &_pARgb_U8[0], &_pARgb_U8[1], &_pARgb_U8[2]);
        _pARgb_U8[3] = 0xFF;
        _pARgb_U8 += 4;

        y_U8 = _pYuYv_U8[3];
        Bof_YuvToRgbReference(y_U8, u_U8, v_U8, &_pARgb_U8[0], &_pARgb_U8[1], &_pARgb_U8[2]);
        _pARgb_U8[3] = 0xFF;
        _pARgb_U8 += 4;
        _pYuYv_U8 += 4;
      }
      if (IncARgb_U32)
      {
        _pARgb_U8 += IncARgb_U32;
      }
      if (IncYuYv_U32)
      {
        _pYuYv_U8 += IncYuYv_U32;
      }
      if (_Height_i < 0)
      {
        _pYuYv_U8 = pStartOfYuYv_U8 - _YuYvStride_U32;
      }
    }
  }
#endif
  return Rts_E;
}

BOFERR Bof_BgraToUyvy(uint32_t _Width_U32, int _Height_i, uint32_t _BrgaStride_U32, const uint8_t *_pBgra_U8, uint32_t _UyvyStride_U32, BOF_RECT *_pCrop_X, BOF_ROTATION _Rotation_E, uint8_t *_pUyvy_U8)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;  // BOF_ERR_EINVAL;
  BOF_RECT r_X(0, 0, _Width_U32, _Height_i);
  int      DstStrideBgra_i, DstStrideUyvy_i, Sts_i, DstWidth_i, DstHeight_i;
  uint8_t *pBgra_U8;

  /*
  if ((_pCrop_X) && (_pCrop_X->IsInside(r_X)))
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  else
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  if (Rts_E == BOF_ERR_NO_ERROR)
  */
  {
    DstStrideBgra_i = _BrgaStride_U32;
    DstStrideUyvy_i = _UyvyStride_U32;
    DstWidth_i = _Width_U32;
    DstHeight_i = _Height_i;
    if (_Rotation_E != BOF_ROTATION::BOF_ROTATION_0)
    {
      if ((_Rotation_E != BOF_ROTATION::BOF_ROTATION_90) || (_Rotation_E != BOF_ROTATION::BOF_ROTATION_180))
      {
        DstStrideBgra_i = _pCrop_X ? _pCrop_X->Height() * 4 : _Height_i * 4;
        DstStrideUyvy_i = _pCrop_X ? _pCrop_X->Height() * 2 : _Height_i * 2;
        DstWidth_i = _pCrop_X ? _pCrop_X->Height() : _Height_i;
        DstHeight_i = _pCrop_X ? _pCrop_X->Width() : _Width_U32;
      }
    }

    pBgra_U8 = nullptr;
    if (_pCrop_X)
    {
      pBgra_U8 = new uint8_t[_pCrop_X->Width() * _pCrop_X->Height() * 4];
      Rts_E = BOF_ERR_ENOMEM;
      if (pBgra_U8)
      {
        Rts_E = BOF_ERR_INIT;
        Sts_i = libyuv::ConvertToARGB(_pBgra_U8, 0, pBgra_U8, DstStrideBgra_i, _pCrop_X->Left_S32, _pCrop_X->Top_S32, _Width_U32, _Height_i, _pCrop_X->Width(), _pCrop_X->Height(), static_cast<libyuv::RotationMode>(_Rotation_E), libyuv::FOURCC_ABGR); //FOURCC_BGRA);
        if (Sts_i == 0)
        {
          /*
                    intptr_t Io;
                    BOFERR Sts_E = Bof_CreateFile(BOF::BOF_FILE_PERMISSION_READ_FOR_ALL | BOF::BOF_FILE_PERMISSION_WRITE_FOR_ALL, "./crop.raw", false, Io);
                    if (Sts_E == BOF_ERR_NO_ERROR)
                    {
                      uint32_t Size_U32=_pCrop_X->Width()*_pCrop_X->Height()*4;
                      Sts_E    = BOF::Bof_WriteFile(Io, Size_U32, pBgra_U8);
                      Sts_E    = BOF::Bof_CloseFile(Io);
                    }
                    */
          Sts_i = libyuv::ARGBToUYVY(pBgra_U8, DstStrideBgra_i, _pUyvy_U8, DstStrideUyvy_i, DstWidth_i, DstHeight_i);
          if (Sts_i == 0)
          {
            Rts_E = BOF_ERR_NO_ERROR;
          }
        }
        BOF_SAFE_DELETE_ARRAY(pBgra_U8);
      }
    }
    else
    {
      Rts_E = BOF_ERR_INIT;
      Sts_i = libyuv::ARGBToUYVY(_pBgra_U8, _BrgaStride_U32, _pUyvy_U8, _UyvyStride_U32, _Width_U32, _Height_i);
      if (Sts_i == 0)
      {
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
  return Rts_E;
}

END_BOF2D_NAMESPACE()