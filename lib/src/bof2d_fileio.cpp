/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the bof module interface to read media
 *
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * History:
 *
 * V 1.00  Nov 13 2022  BHA : Initial release
 */
//#include <2d/include/bof2d/bof2d_fileio.h>
#include <bof2d/bof2d_fileio.h>
#include <bofstd/boffs.h>
#include <bofstd/bofstring.h>

BEGIN_BOF2D_NAMESPACE()

/*!
   Description
   The LoadGraphicFile function loads a .bmp or .tga graphic file

   Parameters
   _rPath: Specifies the graphic filename to open
   _rBmInfo_X: \Returns the graphic file characteristics
   _pPaletteEntry_X: if not nullptr \returns palette table
   _pData_UB: if nullptr just returns the graphic file characteristics in pBmInfo_X
   and _pPaletteEntry_X otherwise store the graphic pixel data
   _LoadIndex_U32: Return the graphic data stored at offset ((FileHEaderSize)+RecordIndex_U32*(*pSize_U32)).
   This feature is usefull to load big graphic file in a small buffer in several passes
   _rSize_U32:   Specifies and returns the number of data read inside the pData_UB buffer. If nullptr
   the buffer is supposed to be large enought to read the data.

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_LoadGraphicFile(BOF::BofPath &_rPath, BOF_BITMAP_INFO_HEADER &_rBmInfo_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32)
{
  BOFERR         Rts_E;
  BOF_TGA_HEADER TgaHeader_X;

  Rts_E = Bof_LoadBmpFile(_rPath, _rBmInfo_X, _pPaletteEntry_X, pData_UB, _LoadIndex_U32, _rSize_U32);
  if (Rts_E != BOF_ERR_NO_ERROR)
  {
    Rts_E = Bof_LoadTgaFile(_rPath, TgaHeader_X, _pPaletteEntry_X, pData_UB, _LoadIndex_U32, _rSize_U32);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      _rBmInfo_X.Reset();
      _rBmInfo_X.Size_U32 = sizeof(BOF_BITMAP_INFO_HEADER);
      _rBmInfo_X.BitCount_U16 = TgaHeader_X.Bits_U8;
      _rBmInfo_X.Height_S32 = TgaHeader_X.Height_U16;
      _rBmInfo_X.Planes_U16 = 1;
      _rBmInfo_X.Width_S32 = TgaHeader_X.Width_U16;
      _rBmInfo_X.ClrUsed_U32 = TgaHeader_X.ColourMapLength_U16;
      _rBmInfo_X.SizeImage_U32 = (TgaHeader_X.Width_U16 * TgaHeader_X.Height_U16 * (TgaHeader_X.Bits_U8 >> 3));
    }
  }
  return (Rts_E);
}

/*!
   Description
   The LoadBmpFile function loads a .bmp graphic file

   Parameters
   _rPath: Specifies the graphic filename to open
   _rBmInfo_X: \Returns the graphic file characteristics
   _pPaletteEntry_X: if not nullptr \returns palette table if needed
   _pData_UB: if nullptr just returns the graphic file characteristics in pBmInfo_X
   and _pPaletteEntry_X otherwise store the graphic pixel data
   _LoadIndex_U32: Return the graphic data stored at offset ((FileHEaderSize)+RecordIndex_U32*(*pSize_U32)).
   This feature is usefull to load big graphic file in a small buffer in several passes
   _rSize_U32:   Specifies and returns the number of data read inside the pData_UB buffer. If nullptr
   the buffer is supposed to be large enought to read the data.

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_LoadBmpFile(BOF::BofPath &_rPath, BOF_BITMAP_INFO_HEADER &_rBmInfo_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *_pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32)
{
  BOFERR                 Rts_E;
  intptr_t               Io;
  uint32_t               i_U32, Width_U32;
  uint32_t               Nb_U32, Size_U32;
  uint8_t                Val_UB;
  BOF_BITMAP_FILE_HEADER BmFile_X;

  Rts_E = Bof_OpenFile(_rPath, true, Io);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Nb_U32 = sizeof(BOF_BITMAP_FILE_HEADER);
    Rts_E = BOF::Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(&BmFile_X));
    if ((Rts_E == BOF_ERR_NO_ERROR)
        && (Nb_U32 == sizeof(BOF_BITMAP_FILE_HEADER))
        && (BmFile_X.Type_U16 == 0x4D42)              // 'BM
        )
    {
      Nb_U32 = sizeof(BOF_BITMAP_INFO_HEADER);
      Rts_E = BOF::Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(&_rBmInfo_X));
      if ((Rts_E == BOF_ERR_NO_ERROR)
          && (Nb_U32 == sizeof(BOF_BITMAP_INFO_HEADER))
          && (_rBmInfo_X.Size_U32 == sizeof(BOF_BITMAP_INFO_HEADER))
          && (_rBmInfo_X.Compression_U32 == BOF_BI_RGB)
          && (_rBmInfo_X.Planes_U16 == 1)
          )
      {
        if (_rBmInfo_X.BitCount_U16 == 8)
        {
          if (!_rBmInfo_X.ClrUsed_U32)
          {
            _rBmInfo_X.ClrUsed_U32 = 256;
          }
          Size_U32 = _rBmInfo_X.ClrUsed_U32 * static_cast<uint32_t>(sizeof(BOF_PALETTE_ENTRY));
          if (_pPaletteEntry_X)
          {
            Nb_U32 = Size_U32;
            Rts_E = BOF::Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(_pPaletteEntry_X));
            if ((Rts_E == BOF_ERR_NO_ERROR)
                && (Nb_U32 == Size_U32)
                )
            {
              // the palette is stored as RGBQUAD array instead of BOF_PALETTE_ENTRY->swap r & b
              for (i_U32 = 0; i_U32 < _rBmInfo_X.ClrUsed_U32; i_U32++)
              {
                Val_UB = _pPaletteEntry_X[i_U32].b_U8;
                _pPaletteEntry_X[i_U32].b_U8 = _pPaletteEntry_X[i_U32].r_U8;
                _pPaletteEntry_X[i_U32].r_U8 = Val_UB;
                _pPaletteEntry_X[i_U32].Flg_U8 = 0xFF; // Opaque
              }
            }
            else
            {
              Rts_E = BOF_ERR_FORMAT;
            }
          }
          else
          {
            Rts_E = (BOF::Bof_SetFileIoPosition(Io, Size_U32, BOF::BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
          }
        }
        else
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          // Bitmap line length are aligned on 4 bytes
          Width_U32 = (_rBmInfo_X.BitCount_U16 >= 8) ? _rBmInfo_X.Width_S32 * (_rBmInfo_X.BitCount_U16 >> 3) : _rBmInfo_X.Width_S32 * (8 / _rBmInfo_X.BitCount_U16);
          if (_rBmInfo_X.Width_S32 & 0x03)
          {
            Width_U32 += (4 - (_rBmInfo_X.Width_S32 & 0x03));
          }
          _rBmInfo_X.Width_S32 = Width_U32 / (_rBmInfo_X.BitCount_U16 >> 3);

          Rts_E = Bof_ReadGraphicFile(Io, _LoadIndex_U32, (_rBmInfo_X.Height_S32 < 0) ? true : false, (uint8_t)_rBmInfo_X.BitCount_U16, _pData_UB, _rBmInfo_X.Width_S32, _rBmInfo_X.Height_S32,
                                      &_rSize_U32);
        }
      }
      else
      {
        Rts_E = BOF_ERR_FORMAT;
      }
    }
    else
    {
      Rts_E = BOF_ERR_FORMAT;
    }
    BOF::Bof_CloseFile(Io);
  }
  return (Rts_E);
}

/*!
   Description
   The LoadTgaFile function loads a .tga graphic file

   Parameters
   _rPath: Specifies the graphic filename to open
   _rTgaHeader_X: \Returns the graphic file characteristics
   _pPaletteEntry_X: if not nullptr \returns palette table if needed
   _pData_UB: if nullptr just returns the graphic file characteristics in pTgaHeader_X
   and _pPaletteEntry_X otherwise store the graphic pixel data
   _LoadIndex_U32: Return the graphic data stored at offset ((FileHEaderSize)+RecordIndex_U32*(*pSize_U32)).
   This feature is usefull to load big graphic file in a small buffer in several passes
   _rSize_U32:   Specifies and returns the number of data read inside the pData_UB buffer. If nullptr
   the buffer is supposed to be large enough to read the data.

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_LoadTgaFile(BOF::BofPath &_rPath, BOF_TGA_HEADER &_rTgaHeader_X, BOF_PALETTE_ENTRY *_pPaletteEntry_X, uint8_t *_pData_UB, uint32_t _LoadIndex_U32, uint32_t &_rSize_U32)
{
  BOFERR   Rts_E;
  intptr_t Io;
  uint32_t i_U32;
  uint32_t Nb_U32, Size_U32;
  uint8_t  pPalette_UB[8];

  Rts_E = Bof_OpenFile(_rPath, true, Io);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Nb_U32 = sizeof(BOF_TGA_HEADER);

    Rts_E = BOF::Bof_ReadFile(Io, Nb_U32, reinterpret_cast<uint8_t *>(&_rTgaHeader_X));
    if ((Rts_E == BOF_ERR_NO_ERROR)
        && (Nb_U32 == sizeof(BOF_TGA_HEADER))
        && (_rTgaHeader_X.ColourMapType_U8 <= 8)
        )
    {
      Rts_E = (BOF::Bof_SetFileIoPosition(Io, _rTgaHeader_X.IdentSize_U8, BOF::BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        if (_rTgaHeader_X.ColourMapType_U8)
        {
          Size_U32 = (_rTgaHeader_X.ColourMapBits_U8 >> 3);
          if (_pPaletteEntry_X)
          {
            for (i_U32 = 0; i_U32 < _rTgaHeader_X.ColourMapLength_U16; i_U32++, _pPaletteEntry_X++)
            {
              Nb_U32 = Size_U32;
              Rts_E = BOF::Bof_ReadFile(Io, Nb_U32, pPalette_UB);
              if ((Rts_E == BOF_ERR_NO_ERROR)
                  && (Nb_U32 == Size_U32)
                  )
              {
                switch (Size_U32)
                {
                  case 3:
                    _pPaletteEntry_X->r_U8 = pPalette_UB[2];
                    _pPaletteEntry_X->g_U8 = pPalette_UB[1];
                    _pPaletteEntry_X->b_U8 = pPalette_UB[0];
                    _pPaletteEntry_X->Flg_U8 = 0xFF;
                    break;

                  case 4:
                    _pPaletteEntry_X->r_U8 = pPalette_UB[2];
                    _pPaletteEntry_X->g_U8 = pPalette_UB[1];
                    _pPaletteEntry_X->b_U8 = pPalette_UB[0];
                    _pPaletteEntry_X->Flg_U8 = pPalette_UB[3];
                    break;

                  default:
                    Rts_E = BOF_ERR_EINVAL;
                    break;
                }
              }
              else
              {
                Rts_E = BOF_ERR_READ;
              }
              if (Rts_E != BOF_ERR_NO_ERROR)
              {
                break;
              }
            }
          }
          else
          {
            Rts_E = (BOF::Bof_SetFileIoPosition(Io, (Size_U32 * _rTgaHeader_X.ColourMapLength_U16), BOF::BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
          }
        }
        else
        {
          Rts_E = BOF_ERR_NO_ERROR;
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          if (_rTgaHeader_X.ImageType_U8 > 3)
          {
            Rts_E = BOF_ERR_BAD_TYPE;   // Accept only uncompressed image
          }
          else
          {
            Rts_E = Bof_ReadGraphicFile(Io, _LoadIndex_U32, (_rTgaHeader_X.Descriptor_X.Origin_UB != 0) ? true : false, static_cast<uint8_t>(_rTgaHeader_X.Bits_U8 >> 3), _pData_UB,
                                        _rTgaHeader_X.Width_U16, _rTgaHeader_X.Height_U16, &_rSize_U32);
          }
        }
      }
    }
    else
    {
      Rts_E = BOF_ERR_FORMAT;
    }
    BOF::Bof_CloseFile(Io);
  }

  return (Rts_E);
}

/*!
   Description
   The GenerateTgaFile function creates a dummy .tga graphic file

   Parameters
   pFn_c: Specifies the graphic filename to create
   Width_U32: Specifies the graphic width
   Height_U32: Specifies the graphic height

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */


BOFERR Bof_GenerateTgaFile(BOF::BofPath &_rPath, uint32_t _Width_U32, uint32_t _Height_U32)
{
  BOF_TGA_HEADER TgaHeader_X;
  BOFERR         Rts_E = BOF_ERR_ENOMEM;
  intptr_t       Io;
  uint32_t       x_U32, y_U32, v_U32, *p_U32, w_U32;
  uint32_t       Nb_U32, Size_U32;
  uint8_t *pData_UB;

  Size_U32 = _Height_U32 * (_Width_U32 << 2);
  if ((pData_UB = new uint8_t[(uint32_t)Size_U32]) != nullptr)
  {
    Rts_E = Bof_CreateFile(BOF::BOF_FILE_PERMISSION_ALL_FOR_ALL, _rPath, false, Io);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      TgaHeader_X.Reset();
      TgaHeader_X.Bits_U8 = 32;
      TgaHeader_X.ImageType_U8 = 2;
      TgaHeader_X.Descriptor_X.Origin_UB = 1;
      TgaHeader_X.Height_U16 = (uint16_t)_Height_U32;
      TgaHeader_X.Width_U16 = (uint16_t)_Width_U32;
      Nb_U32 = sizeof(BOF_TGA_HEADER);
      Rts_E = BOF::Bof_WriteFile(Io, Nb_U32, reinterpret_cast<const uint8_t *>(&TgaHeader_X));
      if ((Rts_E == BOF_ERR_NO_ERROR)
          && (Nb_U32 == sizeof(BOF_TGA_HEADER))
          )
      {
        Rts_E = (BOF::Bof_SetFileIoPosition(Io, TgaHeader_X.IdentSize_U8, BOF::BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          memset(pData_UB, 0, (size_t)Size_U32);
          p_U32 = (uint32_t *)pData_UB;
          for (y_U32 = 0; y_U32 < TgaHeader_X.Height_U16; y_U32++, p_U32 += _Width_U32)
          {
            w_U32 = TgaHeader_X.Width_U16 - 1;
            v_U32 = 0xFFFF0000;
            for (x_U32 = 0; x_U32 < w_U32; x_U32++)
            {
              if (x_U32 == (w_U32 - 1))
              {
                v_U32 &= 0x00FFFFFF;
                v_U32 |= 0x80000000;
              }
              p_U32[x_U32] = v_U32;
            }
          }
          Nb_U32 = Size_U32;
          Rts_E = BOF::Bof_WriteFile(Io, Nb_U32, pData_UB);
          if ((Rts_E == BOF_ERR_NO_ERROR)
              && (Nb_U32 == Size_U32)
              )
          {
          }
          else
          {
            Rts_E = BOF_ERR_WRITE;
          }
        }
      }
      else
      {
        Rts_E = BOF_ERR_READ;
      }
      BOF::Bof_CloseFile(Io);
    }
    BOF_SAFE_DELETE_ARRAY(pData_UB);
    //		delete pData_UB;
  }
  return (Rts_E);
}

/*!
   Description
   The ReadGraphicFile function reads the data part of a tga or bmp file. These one can be
   Top_S32 down or down Top_S32 oriented

   Parameters
   Io: Specifies the file handle. The file read pointer must be located on the first byte of
   the graphic file data zone
   _RecordIndex_U32: Specifies the record index inside the file data zone to access. The file data zone is view as a list of
   successive binary record of length
   TopDown_B: true if the data byte in the file are Top_S32 down oriented
   _BytePerPixel_UB: Specifies the number of byte per pixel
   _pData_UB: Specifies a pointer to the buffer which will contain the data read.
   _Width_U32: Specifies the graphic width
   _Height_U32: Specifies the graphic height
   _rRecordSize_U32. Specifies the size of a data record. This information is used with RecordIndex_U32 and
   the TopDown_B argument to compute the file offset to read
   The data really read are also returned to the called via this pointer

   Returns
   bool: true if the operation is successful

   Remarks
   None
 */

BOFERR Bof_ReadGraphicFile(intptr_t Io, uint32_t _RecordIndex_U32, bool _TopDown_B, uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, uint32_t *_pRecordSize_U32)
{
  BOFERR   Rts_E = BOF_ERR_EINVAL;
  uint32_t NbLineToRead_U32, i_U32, LineInFile_U32, WidthInByte_U32;
  uint32_t Size_U32, Nb_U32;
  uint8_t *p_UB;

  if (_pData_UB)
  {
    WidthInByte_U32 = _Width_U32 * _BytePerPixel_UB;
    if (_pRecordSize_U32)
    {
      NbLineToRead_U32 = (*_pRecordSize_U32 / WidthInByte_U32);
      if (NbLineToRead_U32 > _Height_U32)
      {
        NbLineToRead_U32 = _Height_U32;
      }

      if (_TopDown_B)
      {
        LineInFile_U32 = _RecordIndex_U32 * NbLineToRead_U32;
        if (LineInFile_U32 < _Height_U32)
        {
          if ((LineInFile_U32 + NbLineToRead_U32) >= _Height_U32)
          {
            NbLineToRead_U32 = _Height_U32 - LineInFile_U32;
          }
        }
      }
      else
      {
        LineInFile_U32 = _Height_U32 - ((_RecordIndex_U32 + 1) * NbLineToRead_U32);
        if (LineInFile_U32 & 0x80000000)
        {
          NbLineToRead_U32 = _Height_U32 + LineInFile_U32; // LineInFile_U32 is <0
          LineInFile_U32 = 0;
        }
      }

      if ((!(LineInFile_U32 & 0x80000000))
          && (!(NbLineToRead_U32 & 0x80000000))
          && (LineInFile_U32 < _Height_U32)
          )
      {
        Rts_E = (BOF::Bof_SetFileIoPosition(Io, LineInFile_U32 * WidthInByte_U32, BOF::BOF_SEEK_METHOD::BOF_SEEK_CURRENT) != (int64_t)-1) ? BOF_ERR_NO_ERROR : BOF_ERR_SEEK;
      }
      *_pRecordSize_U32 = 0;
    }
    else
    {
      NbLineToRead_U32 = _Height_U32;
      Rts_E = BOF_ERR_NO_ERROR;
    }

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Size_U32 = 0;
      if (_TopDown_B)
      {
        Size_U32 = WidthInByte_U32 * NbLineToRead_U32;
        Nb_U32 = Size_U32;
        Rts_E = BOF::Bof_ReadFile(Io, Nb_U32, _pData_UB);
        if ((Rts_E == BOF_ERR_NO_ERROR)
            && (Nb_U32 == Size_U32)
            )
        {
        }
        else
        {
          Rts_E = BOF_ERR_READ;
        }
      }
      else
      {
        //Nb_U32 = 0;
        p_UB = (_pData_UB)+((NbLineToRead_U32 - 1) * WidthInByte_U32);
        for (i_U32 = 0; i_U32 < NbLineToRead_U32; i_U32++)
        {
          Nb_U32 = WidthInByte_U32;
          if ((BOF::Bof_ReadFile(Io, Nb_U32, p_UB) != 0)
              || (Nb_U32 != WidthInByte_U32)
              )
          {
            Size_U32 += Nb_U32;
            break;
          }
          else
          {
            p_UB -= WidthInByte_U32;
          }
          Size_U32 += Nb_U32;
        }
        Rts_E = (i_U32 == NbLineToRead_U32) ? BOF_ERR_NO_ERROR : BOF_ERR_READ;
      }
      if (_pRecordSize_U32)
      {
        *_pRecordSize_U32 = (uint32_t)Size_U32;
      }
    }
  }
  else
  {
    Rts_E = BOF_ERR_NO_ERROR;
  }
  return (Rts_E);
}
END_BOF2D_NAMESPACE()