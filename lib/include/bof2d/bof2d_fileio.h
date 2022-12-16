/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof module interface to read media
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
#include <2d/include/bof2d/bof2d.h>

#include <bofstd/bofpath.h>

BEGIN_BOF2D_NAMESPACE()

#define BOF_BI_RGB          0L
#define BOF_BI_RLE8         1L
#define BOF_BI_RLE4         2L
#define BOF_BI_BITFIELDS    3L

#define BOF_TGA_NULL        0
#define BOF_TGA_MAP         1          // uncompressed color mapped
#define BOF_TGA_RGB         2          // uncompressed true color
#define BOF_TGA_MONO        3          // uncompressed grey scale
#define BOF_TGA_RLEMAP      9          // RLE compressed color mapped
#define BOF_TGA_RLERGB      10         // RLE compressed true color
#define BOF_TGA_RLEMONO     11         // RLE compressed grey scale
#define BOF_TGA_COMPMAP     32         // obsolete types
#define BOF_TGA_COMPMAP4    33

// Definitions for interleave flag.
#define BOF_TGA_IL_NONE     0
#define BOF_TGA_IL_TWO      1
#define BOF_TGA_IL_FOUR     2

#pragma pack(2)

struct BOF2D_EXPORT BOF_BITMAP_FILE_HEADER
{
  uint16_t Type_U16;
  uint32_t Size_U32;
  uint16_t Reserved1_U16;
  uint16_t Reserved2_U16;
  uint32_t OffBits_U32;

  BOF_BITMAP_FILE_HEADER()
  {
    Reset();
  }

  void Reset()
  {
    Type_U16 = 0;
    Size_U32 = 0;
    Reserved1_U16 = 0;
    Reserved2_U16 = 0;
    OffBits_U32 = 0;
  }
};

#pragma pack()

#pragma pack (1)

struct BOF2D_EXPORT BOF_BITMAP_INFO_HEADER
{
  uint32_t Size_U32;
  int32_t Width_S32;
  int32_t Height_S32;
  uint16_t Planes_U16;
  uint16_t BitCount_U16;
  uint32_t Compression_U32;
  uint32_t SizeImage_U32;
  int32_t XPelsPerMeter_S32;
  int32_t YPelsPerMeter_S32;
  uint32_t ClrUsed_U32;
  uint32_t ClrImportant_U32;

  BOF_BITMAP_INFO_HEADER()
  {
    Reset();
  }

  void Reset()
  {
    Size_U32 = 0;
    Width_S32 = 0;
    Height_S32 = 0;
    Planes_U16 = 0;
    BitCount_U16 = 0;
    Compression_U32 = 0;
    SizeImage_U32 = 0;
    XPelsPerMeter_S32 = 0;
    YPelsPerMeter_S32 = 0;
    ClrUsed_U32 = 0;
    ClrImportant_U32 = 0;
  }
};

struct BOF2D_EXPORT BOF_TGA_FLAG
{
  uint8_t AttBitsPerPixel_U8 : 4;   // Bits 0-3: 4 bits, number of attribute bits per pixel
  uint8_t Reserved_U8 : 1;   // Bit 4: 1 bit, origin: 0=left, 1=right
  uint8_t Origin_UB : 1;   // Bit 5: 1 bit, origin: 0=lower left, 1=upper left
  uint8_t InterLeaveMode_U8 : 2;   // bit 7-6: 2 bits, interleaving flag 00 = non-interleaved. 01 = two-way (even/odd) interleaving. 10 = four way interleaving.11 = reserved.
  BOF_TGA_FLAG()
  {
    Reset();
  }

  void Reset()
  {
    AttBitsPerPixel_U8 = 0;
    Reserved_U8 = 0;
    Origin_UB = 0;
    InterLeaveMode_U8 = 0;
  }
};

/*
0  -  No image data included.
1  -  Uncompressed, color-mapped images.
2  -  Uncompressed, RGB images.
3  -  Uncompressed, black and white images.
9  -  Runlength encoded color-mapped images.
10 -  Runlength encoded RGB images.
11 -  Compressed, black and white images.
32 -  Compressed color-mapped data, using Huffman, Delta, and
runlength encoding.
33 -  Compressed color-mapped data, using Huffman, Delta, and
runlength encoding.  4-pass quadtree-type process.
*/
struct BOF2D_EXPORT BOF_TGA_HEADER
{
  uint8_t IdentSize_U8;        // size of ID field that follows 18 uint8_t header (0 usually)
  uint8_t ColourMapType_U8;    // type of colour map 0=none, 1=has palette
  uint8_t ImageType_U8;        // type of image 0=none,1=indexed,2=rgb,3=grey,>8=rle packed

  uint16_t ColourMapStart_U16;  // first colour map entry in palette
  uint16_t ColourMapLength_U16; // number of colours in palette
  uint8_t ColourMapBits_U8;    // number of bits per palette entry 15,16,24,32

  uint16_t XStart_U16;          // image x origin
  uint16_t YStart_U16;          // image y origin
  uint16_t Width_U16;           // image width in pixels
  uint16_t Height_U16;          // image height in pixels
  uint8_t Bits_U8;             // image bits per pixel 8,16,24,32

  BOF_TGA_FLAG Descriptor_X;        // image descriptor bits (vh flip bits)
  BOF_TGA_HEADER()
  {
    Reset();
  }

  void Reset()
  {
    IdentSize_U8 = 0;
    ColourMapType_U8 = 0;
    ImageType_U8 = 0;

    ColourMapStart_U16 = 0;
    ColourMapLength_U16 = 0;
    ColourMapBits_U8 = 0;

    XStart_U16 = 0;
    YStart_U16 = 0;
    Width_U16 = 0;
    Height_U16 = 0;
    Bits_U8 = 0;

    Descriptor_X.Reset();
  }
};

#pragma pack ()

BOF2D_EXPORT BOFERR Bof_LoadGraphicFile(BOF::BofPath &_rPath, BOF_BITMAP_INFO_HEADER &_rBmInfo_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32);
BOF2D_EXPORT BOFERR Bof_LoadTgaFile(BOF::BofPath &_rPath, BOF_TGA_HEADER &_rTgaHeader_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *_pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32);
BOF2D_EXPORT BOFERR Bof_GenerateTgaFile(BOF::BofPath &_rPath, uint32_t _Width_U32, uint32_t _Height_U32);
BOF2D_EXPORT BOFERR Bof_LoadBmpFile(BOF::BofPath &_rPath, BOF_BITMAP_INFO_HEADER &_rBmInfo_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *_pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32);
BOF2D_EXPORT BOFERR Bof_ReadGraphicFile(intptr_t Io, uint32_t _RecordIndex_U32, bool _TopDown_B, uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, uint32_t *_pRecordSize_U32);
END_BOF2D_NAMESPACE()