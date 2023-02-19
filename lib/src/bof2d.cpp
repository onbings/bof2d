/*
   Copyright (c) 2000-2006, Sci. All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   This module defines the bof module interface for 2d manip

   Author:      Bernard HARMEL: onbings@gmail.com
   Web:			    onbings.dscloud.me
   Revision:    1.0

   History:

   V 1.00  Sep 30 2000  BHA : Initial release
 */

#include <bof2d/bof2d.h>

BEGIN_BOF2D_NAMESPACE()
//YUV Color
const BOF_YUVA GL_pYuvRainbow75_X[] =
{
  {180, 128, 128, 255},     /* 75% white */
  {162, 44,  142, 255},     /* 75% yellow */
  {131, 156, 44,  255},     /* 75% cyan */
  {112, 72,  58,  255},     /* 75% green */
  {84,  184, 198, 255},     /* 75% magenta */
  {65,  100, 212, 255},     /* 75% red */
  {35,  212, 114, 255},     /* 75% blue */
};

const BOF_YUVA GL_pYuvRainbow100_X[] =
{
  {235, 128, 128, 255},     /* 100% white */
  {210, 16,  146, 255},     /* 100% yellow */
  {170, 166, 16,  255},     /* 100% cyan */
  {145, 54,  34,  255},     /* 100% green */
  {106, 202, 222, 255},     /* 100% magenta */
  {81,  90,  240, 255},     /* 100% red */
  {41,  240, 110, 255},     /* 100% blue */
};

const BOF_YUVA GL_pYuvRainbowHd_X[] =
{
  {180, 128, 128, 255},     /* 75% white */
  {168, 44,  136, 255},     /* 75% yellow */
  {145, 147, 44,  255},     /* 75% cyan */
  {133, 63,  52,  255},     /* 75% green */
  {63,  193, 204, 255},      /* 75% magenta */
  {51,  109, 212, 255},      /* 75% red */
  {28,  212, 120, 255},      /* 75% blue */
};

const BOF_YUVA GL_pYuvWobNair_X[] =
{
  {35,  212, 114, 255},     /* 75% blue */
  {19,  128, 128, 255},     /* 7.5% intensity black */
  {84,  184, 198, 255},     /* 75% magenta */
  {19,  128, 128, 255},     /* 7.5% intensity black */
  {131, 156, 44,  255},     /* 75% cyan */
  {19,  128, 128, 255},     /* 7.5% intensity black */
  {180, 128, 128, 255},     /* 75% white */
};

const BOF_YUVA GL_YuvWhite_X = { 235, 128, 128, 255 };

/* pluge pulses */
const BOF_YUVA GL_YuvNeg4Ire_X = { 7, 128, 128, 255 };
const BOF_YUVA GL_YuvPos4Ire_X = { 24, 128, 128, 255 };

/* fudged Q/-I */
const BOF_YUVA GL_YuvIPixel_X = { 57, 156, 97, 255 };
const BOF_YUVA GL_YuvQPixel_X = { 44, 171, 147, 255 };

const BOF_YUVA GL_YuvGray40_X = { 104, 128, 128, 255 };
const BOF_YUVA GL_YuvGray15_X = { 49, 128, 128, 255 };
const BOF_YUVA GL_YuvCyan_X = { 188, 154, 16, 255 };
const BOF_YUVA GL_YuvYellow_X = { 219, 16, 138, 255 };
const BOF_YUVA GL_YuvRed_X = { 63, 102, 240, 255 };
const BOF_YUVA GL_YuvGreen_X = { 145, 54, 34, 255 };
const BOF_YUVA GL_YuvBlue_X = { 32, 240, 118, 255 };
const BOF_YUVA GL_YuvBlack0_X = { 16, 128, 128, 255 };
const BOF_YUVA GL_YuvBlack2_X = { 20, 128, 128, 255 };
const BOF_YUVA GL_YuvBlack4_X = { 25, 128, 128, 255 };
const BOF_YUVA GL_YuvNeg2_X = { 12, 128, 128, 255 };

/*!
   Description
   The LookForZoneCoordinate function scans a pixel buffer looking for a pixel color. It is
   used to define rectangular zone inside a picture

   Parameters
   _rRect_X: Specifies the rectangular zone to cover inside the pixel buffer and returns the rectangle found.
   This one is defined by the upper Left_S32 and lower Right_S32 corner.
   _BitCount_U32: Specifies the number of bits per pixel
   _pData: Specifies the pixel buffer address
   _Pitch_U32: Specifies the pixel buffer pitch
   _Color_U32: Specifies the pixel color to look for

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */


BOFERR Bof_LookForZoneCoordinate(BOF_RECT &_rRect_X, uint32_t _BitCount_U32, void *_pData, uint32_t _Pitch_U32, uint32_t _Color_U32)
{
  bool     Rts_B = false;
  uint32_t x_U32 = 0, y_U32, *p_U32, Offset_U32, Width_U32, Height_U32, Left_U32, Top_U32;
  //	uint16_t *p_U16;	// , Color_U16;
  //	uint8_t  *p_UB;	// , Color_UB;

  if (_pData)
  {
    //	p_UB       = (uint8_t *)_pData;
//		Color_UB   = (uint8_t)_Color_U32;
    //	p_U16      = (uint16_t *)_pData;
    //Color_U16  = (uint16_t)_Color_U32;
    p_U32 = (uint32_t *)_pData;
    Width_U32 = (_rRect_X.Right_S32 - _rRect_X.Left_S32); // Compatible with rect definition Right_S32,Bottom_S32 is outside of the rect
    Height_U32 = (_rRect_X.Bottom_S32 - _rRect_X.Top_S32);
    Left_U32 = _rRect_X.Left_S32;
    Top_U32 = _rRect_X.Top_S32;
    switch (_BitCount_U32)
    {
      case 8:
        //				p_UB  += ( (_rRect_X.Top_S32 * _Pitch_U32) + _rRect_X.Left_S32);
        break;

      case 16:
        //				p_U16 += ( ( (_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 1) ) >> 1);
        break;

      case 24:
        p_U32 = (uint32_t *)((uint8_t *)p_U32 + ((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 * 3)));
        break;

      default:
      case 32:
        p_U32 += (((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 2)) >> 2);
        break;
    }

    switch (_BitCount_U32)
    {
      case 8:
        Offset_U32 = _Pitch_U32 - Width_U32;
        break;

      case 16:
        Offset_U32 = _Pitch_U32 - (Width_U32 << 1);
        break;

      case 24:
        Offset_U32 = _Pitch_U32 - (Width_U32 * 3);
        break;

      default:
      case 32:
        Offset_U32 = _Pitch_U32 - (Width_U32 << 2);
        break;
    }
    // Get Top_S32,Left_S32
    for (y_U32 = 0; y_U32 < Height_U32; y_U32++)
    {
      switch (_BitCount_U32)
      {
        case 32:
          for (x_U32 = 0; x_U32 < Width_U32; x_U32++, p_U32++)
          {
            if (*p_U32 == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Top_S32 = Top_U32 + y_U32;
              _rRect_X.Left_S32 = Left_U32 + x_U32;
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
              break;
            }
          }
          if (!Rts_B)
          {
            p_U32 = (uint32_t *)((uint8_t *)p_U32 + Offset_U32);
          }
          break;

        case 24:
          for (x_U32 = 0; x_U32 < Width_U32; x_U32++)
          {
            if ((x_U32 < (Width_U32 - 1))
                || (y_U32 < (Height_U32 - 1))
                )                     // Avoid bus error on last pixel (.net)
            {
              if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
              {
                Rts_B = true;
                _rRect_X.Top_S32 = Top_U32 + y_U32;
                _rRect_X.Left_S32 = Left_U32 + x_U32;
                p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
                break;
              }
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + 3);
            }
          }
          if (!Rts_B)
          {
            p_U32 = (uint32_t *)((uint8_t *)p_U32 + Offset_U32);
          }
          break;

        default:
          break;
      }
      if (Rts_B)
      {
        break;
      }
    }

    if (Rts_B)
    {
      Rts_B = false;
      // Get Bottom_S32
      for (y_U32++; y_U32 < Height_U32; y_U32++)
      {
        switch (_BitCount_U32)
        {
          default:
          case 32:
            if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Bottom_S32 = Top_U32 + y_U32;
              p_U32++;
            }
            else
            {
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
            }
            break;

          case 24:
            if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Bottom_S32 = Top_U32 + y_U32;
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + 3);
            }
            else
            {
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
            }
            break;
        }
        if (Rts_B)
        {
          break;
        }
      }
    }
    if (Rts_B)
    {
      Rts_B = false;
      // Get Right_S32
      for (x_U32++; x_U32 < Width_U32; x_U32++)
      {
        switch (_BitCount_U32)
        {
          default:
          case 32:
            if (*p_U32 == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Right_S32 = Left_U32 + x_U32;
            }
            else
            {
              p_U32++;
            }
            break;

          case 24:
            if ((*p_U32 & 0x00FFFFFFFF) == _Color_U32)
            {
              Rts_B = true;
              _rRect_X.Right_S32 = Left_U32 + x_U32;
            }
            else
            {
              p_U32 = (uint32_t *)((uint8_t *)p_U32 + _Pitch_U32);
            }
            break;
        }
        if (Rts_B)
        {
          break;
        }
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}

/*!
   Description
   The EraseZoneBorder function erase the rectangular delimiter present in a pixel buffer

   Parameters
   _rRect_X: Specifies the rectangular zone to erase inside the pixel buffer.
   _BitCount_U32: Specifies the number of bits per pixel
   _pData: Specifies the pixel buffer address
   _Pitch_U32: Specifies the pixel buffer pitch

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_EraseZoneBorder(BOF_RECT &_rRect_X, uint32_t _BitCount_U32, void *_pData, uint32_t _Pitch_U32)
{
  bool     Rts_B = false;
  uint32_t *p_U32;
  //	uint16_t *p_U16;
  //	uint8_t  *p_UB;

  if (_pData)
  {
    //	p_UB  = (uint8_t *)_pData;
    //	p_U16 = (uint16_t *)_pData;
    p_U32 = (uint32_t *)_pData;

    switch (_BitCount_U32)
    {
      case 8:
        //		p_UB  += ( (_rRect_X.Top_S32 * _Pitch_U32) + _rRect_X.Left_S32);
        break;

      case 16:
        //		p_U16 += ( ( (_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 1) ) >> 1);
        break;

      case 24:
        p_U32 = (uint32_t *)((uint8_t *)p_U32 + ((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 * 3)));
        break;
      default:
      case 32:
        p_U32 += (((_rRect_X.Top_S32 * _Pitch_U32) + (_rRect_X.Left_S32 << 2)) >> 2);
        break;
    }
    switch (_BitCount_U32)
    {
      case 32:
        *p_U32 = *(p_U32 + 1);
        p_U32 = (uint32_t *)((uint8_t *)p_U32 + ((_rRect_X.Bottom_S32 - _rRect_X.Top_S32) * _Pitch_U32));
        *p_U32 = *(p_U32 + 1);
        p_U32 += (_rRect_X.Right_S32 - _rRect_X.Left_S32);
        *p_U32 = *(p_U32 - 1);
        break;

      default:
        break;
    }
    Rts_B = true;
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);

}

/*!
   Description
   The DecimateGraphicData function compute a downscaled picture from a full scaled one.

   Parameters
   _BytePerPixel_UB: Specifies the number of byte per pixel
   _pData_UB: Specifies a pointer to the buffer which will contain the data read.
   _Width_U32: Specifies the graphic width
   _Height_U32: Specifies the graphic height
   _DecimationStepX_f: Specifies the X scaling factor (must be >= 1.0f)
   _DecimationStepY_f: Specifies the Y scaling factor (must be >= 1.0f)

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_DecimateGraphicData(uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, float _DecimationStepX_f, float _DecimationStepY_f)
{
  bool     Rts_B = false;
  uint8_t *p_UB;
  uint16_t *p_U16;
  uint32_t *p_U32;
  float    r_F, c_F, Index_F;

  if ((_pData_UB)
      && (_DecimationStepX_f >= 1.0f)
      && (_DecimationStepY_f >= 1.0f)
      )
  {
    Rts_B = true;
    if ((_DecimationStepX_f != 1.0f)
        || (_DecimationStepY_f != 1.0f)
        )
    {
      p_UB = _pData_UB;
      p_U16 = (uint16_t *)_pData_UB;
      p_U32 = (uint32_t *)_pData_UB;
      switch (_BytePerPixel_UB)
      {
        case 1:
          for (r_F = 0.0f; (uint32_t)(r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F = (float)((uint32_t)r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t)(c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_UB++ = _pData_UB[(uint32_t)(Index_F + 0.5f)];
              Index_F += _DecimationStepX_f;
            }
          }
          break;

        case 2:
          for (r_F = 0.0f; (uint32_t)(r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F = (float)((uint32_t)r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t)(c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_U16++ = ((uint16_t *)_pData_UB)[(uint32_t)(Index_F + 0.5f)];
              Index_F += _DecimationStepX_f;
            }
          }
          break;

        case 4:
          for (r_F = 0.0f; (uint32_t)(r_F + 0.5f) < _Height_U32; r_F += _DecimationStepY_f)
          {
            Index_F = (float)((uint32_t)r_F * _Width_U32);
            for (c_F = 0.0f; (uint32_t)(c_F + 0.5f) < _Width_U32; c_F += _DecimationStepX_f)
            {
              *p_U32++ = ((uint32_t *)_pData_UB)[(uint32_t)(Index_F + 0.5f)];
              Index_F += _DecimationStepX_f;
            }
          }
          break;

        default:
          Rts_B = false;
          break;
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}

/*!
   Description
   The LookForColor function scans a color table looking for a particula value

   Parameters
   _NbColorEntry_U32: Specifies the number of colors in the pColorTable_X array
   _pColorTable_X: Specifies the color table array
   _Color_X: Specifies the color to look for
   _pColorEntry_U32: \Returns the color table entry found (-1 if no match)

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_LookForColor(uint32_t _NbColorEntry_U32, BOF_PALETTE_ENTRY *_pColorTable_X, BOF_PALETTE_ENTRY _Color_X, uint32_t *_pColorEntry_U32)
{
  bool     Rts_B = false;
  uint32_t i_U32;

  if ((_pColorTable_X) && (_pColorEntry_U32))
  {
    *_pColorEntry_U32 = (uint32_t)-1;
    for (i_U32 = 0; i_U32 < _NbColorEntry_U32; i_U32++, _pColorTable_X++)
    {
      if ((_pColorTable_X->r_U8 == _Color_X.r_U8)
          && (_pColorTable_X->g_U8 == _Color_X.g_U8)
          && (_pColorTable_X->b_U8 == _Color_X.b_U8)
          )
      {
        Rts_B = true;
        *_pColorEntry_U32 = i_U32;
        break;
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}

/*!
   Description
   The SwapColorEntries function parse a paletized picture data buffer and swap two
   pixel color (For example each red pixel is turned into blue and each blue pixel is turned into red)

   Parameters
   _BytePerPixel_UB: Specifies the number of byte per pixel
   _pData_UB: Specifies a pointer to the buffer which will contain the data read.
   _Width_U32: Specifies the graphic width
   _Height_U32: Specifies the graphic height
   _ColorEntry1_U32. Specifies the first color to swap
   _ColorEntry2_U32. Specifies the first color to swap

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_SwapColorEntries(uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, uint32_t _ColorEntry1_U32, uint32_t _ColorEntry2_U32)
{
  bool     Rts_B = false;
  uint32_t i_U32, j_U32;
  uint8_t *p_UB;
  uint16_t *p_U16;
  uint32_t *p_U32;

  if (_pData_UB)
  {
    Rts_B = true;
    if (_ColorEntry1_U32 != _ColorEntry2_U32)
    {
      p_UB = _pData_UB;
      p_U16 = (uint16_t *)_pData_UB;
      p_U32 = (uint32_t *)_pData_UB;
      switch (_BytePerPixel_UB)
      {
        case 1:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_UB++)
            {
              if (*p_UB == (uint8_t)_ColorEntry1_U32)
              {
                *p_UB = (uint8_t)_ColorEntry2_U32;
              }
              else if (*p_UB == (uint8_t)_ColorEntry2_U32)
              {
                *p_UB = (uint8_t)_ColorEntry1_U32;
              }
            }
          }
          break;

        case 2:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_U16++)
            {
              if (*p_U16 == (uint16_t)_ColorEntry1_U32)
              {
                *p_U16 = (uint16_t)_ColorEntry2_U32;
              }
              else if (*p_U16 == (uint16_t)_ColorEntry2_U32)
              {
                *p_U16 = (uint16_t)_ColorEntry1_U32;
              }
            }
          }
          break;

        case 4:
          for (i_U32 = 0; i_U32 < _Height_U32; i_U32++)
          {
            for (j_U32 = 0; j_U32 < _Width_U32; j_U32++, p_U32++)
            {
              if (*p_U32 == _ColorEntry1_U32)
              {
                *p_U32 = _ColorEntry2_U32;
              }
              else if (*p_U32 == _ColorEntry2_U32)
              {
                *p_U32 = _ColorEntry1_U32;
              }
            }
          }
          break;

        default:
          Rts_B = false;
          break;
      }
    }
  }
  return (Rts_B ? BOF_ERR_NO_ERROR : BOF_ERR_EINVAL);
}
END_BOF2D_NAMESPACE()