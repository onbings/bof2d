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
//#include <bof2d/bof2d.h>
#pragma message("!!!!!!!!!!!!! CPP => TODO ADAPT THIS LINE AND REMOVE FILE FORM bofstd !!!!!!!!!!!!!")
#include "../include/bof2d/bof2d.h"

BEGIN_BOF2D_NAMESPACE()

BOF2D_EXPORT BOFERR Bof_Rgb2Yuv(uint8_t _r_UB, uint8_t _g_UB, uint8_t _b_UB, uint8_t &_rY_UB, uint8_t &_rU_UB, uint8_t &_rV_UB);
BOF2D_EXPORT BOFERR Bof_Yuv2Rgb(uint8_t _y_UB, uint8_t _u_UB, uint8_t _v_UB, uint8_t &_rR_UB, uint8_t &_rG_UB, uint8_t &_rB_UB);
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

END_BOF2D_NAMESPACE()