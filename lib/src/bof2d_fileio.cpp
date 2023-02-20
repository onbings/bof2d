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
#include <bofstd/boffs.h>
#include <bofstd/bofstring.h>

#include <bof2d/bof2d_fileio.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <SDL.h>        

BEGIN_BOF2D_NAMESPACE()
//https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
BOF2D_EXPORT BOFERR Bof_ReadGraphicFile(const BOF::BofPath& _rPath, int _NbChannelToRead_i, BOF_FRAME_DATA& _rFrameData_X)
{
  BOFERR Rts_E = BOF_ERR_FORMAT;
  uint8_t* pFrameData_U8;
  uint32_t FrameSize_U32;
  int Width_i, Height_i, NbChannel_i, FileCanbeRead_B;

  FileCanbeRead_B = stbi_info(_rPath.FullPathName(false).c_str(), &Width_i, &Height_i, &NbChannel_i);
  if (FileCanbeRead_B)
  {
    _rFrameData_X.FrameSizeInPixel_X.Width_U32 = Width_i;
    _rFrameData_X.FrameSizeInPixel_X.Height_U32 = Height_i;
    _rFrameData_X.NbChannel_U32 = NbChannel_i;
    _rFrameData_X.LineSizeInByte_U32 = (_NbChannelToRead_i == 0) ? _rFrameData_X.FrameSizeInPixel_X.Width_U32 * _rFrameData_X.NbChannel_U32 : _rFrameData_X.FrameSizeInPixel_X.Width_U32 * _NbChannelToRead_i;
    FrameSize_U32 = _rFrameData_X.LineSizeInByte_U32 * _rFrameData_X.FrameSizeInPixel_X.Height_U32;
     Rts_E = BOF_ERR_EINVAL;
    if ((_rFrameData_X.FrameBuffer_X.pData_U8==nullptr) && (_rFrameData_X.FrameBuffer_X.Capacity_U64==0) && (_rFrameData_X.FrameBuffer_X.Size_U64 == 0))
    {
      Rts_E = BOF_ERR_READ;
      pFrameData_U8 = stbi_load(_rPath.FullPathName(false).c_str(), &Width_i, &Height_i, &NbChannel_i, _NbChannelToRead_i);
      if (pFrameData_U8)
      {
        BOF_ASSERT(Width_i == _rFrameData_X.FrameSizeInPixel_X.Width_U32);
        BOF_ASSERT(Height_i == _rFrameData_X.FrameSizeInPixel_X.Height_U32);
        BOF_ASSERT(NbChannel_i == _rFrameData_X.NbChannel_U32);
        _rFrameData_X.FrameBuffer_X.SetStorage(FrameSize_U32, FrameSize_U32, pFrameData_U8);
        _rFrameData_X.FrameBuffer_X.MustBeFreeed_B = true;    //stbi_image_free(pFrameData_U8);
        Rts_E = BOF_ERR_NO_ERROR;
      }
    }
  }
  return Rts_E;
}
BOF2D_EXPORT BOFERR Bof_WriteGraphicFile(BOF2D_AV_VIDEO_FORMAT _Format_E, const BOF::BofPath& _rPath, uint32_t _EncQuality_U32, const BOF_FRAME_DATA& _rFrameData_X)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;

  if ((_EncQuality_U32 <= 100) && (_rFrameData_X.FrameBuffer_X.pData_U8) && (_rFrameData_X.FrameBuffer_X.Size_U64))
  {
    switch (_Format_E)
    {
    case BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_BMP:
      Rts_E = (stbi_write_bmp(_rPath.FullPathName(false).c_str(), _rFrameData_X.FrameSizeInPixel_X.Width_U32, _rFrameData_X.FrameSizeInPixel_X.Height_U32, _rFrameData_X.NbChannel_U32, _rFrameData_X.FrameBuffer_X.pData_U8) == 0) ? BOF_ERR_INTERNAL : BOF_ERR_NO_ERROR;
      break;

    case BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_PNG:
      Rts_E = BOF_ERR_NO_ERROR;
      //int stbi_write_tga_with_rle;             // defaults to true; set to 0 to disable RLE
  /*
  STBIW_ZLIB_COMPRESS: Use Zlib:
     The compression level must be Z_DEFAULT_COMPRESSION, or between 0 and 9:
     1 gives best speed, 9 gives best compression, 0 gives no compression at all
     (the input data is simply copied a block at a time).  Z_DEFAULT_COMPRESSION
     requests a default compromise between speed and compression (currently
     equivalent to level 6).

  */
      stbi_write_png_compression_level = (_EncQuality_U32 / 10);    //if not zlib: defaults to 8; set to higher for more compression
      //https://optipng.sourceforge.net/pngtech/png_optimization.html#:~:text=The%20PNG%20format%20employs%20five,and%2For%20the%20upper%20left.
      stbi_write_force_png_filter = -1;
      /*
      Most of the parameters are the same ones that are also passed to stbi_write_png which has a documentation in the header file:

      pixels is the uncompressed image data
      stride_bytes is the number of bytes between two the start of two consecutive rows:
      "stride_in_bytes" is the distance in bytes from the first byte of a row of pixels to the first byte of the next row of pixels.

      x (called w in stbi_write_png) is the width of the image
      y (called h in stbi_write_png) is the height of the image
      n (called comp in stbi_write_png) is the number of channels:
      Each pixel contains 'comp' channels of data stored interleaved with 8-bits per channel, in the following order: 1=Y, 2=YA, 3=RGB, 4=RGBA.

      out_len is a output parameter which returns the number of bytes the compressed data has.
      */
      Rts_E = (stbi_write_png(_rPath.FullPathName(false).c_str(), _rFrameData_X.FrameSizeInPixel_X.Width_U32, _rFrameData_X.FrameSizeInPixel_X.Height_U32, _rFrameData_X.NbChannel_U32, _rFrameData_X.FrameBuffer_X.pData_U8, _rFrameData_X.LineSizeInByte_U32) == 0) ? BOF_ERR_INTERNAL : BOF_ERR_NO_ERROR;
      break;

    case BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_TGA:
      Rts_E = BOF_ERR_NO_ERROR;
      stbi_write_tga_with_rle = (_EncQuality_U32 != 0) ? true : false;             // defaults to true; set to 0 to disable RLE
      Rts_E = (stbi_write_tga(_rPath.FullPathName(false).c_str(), _rFrameData_X.FrameSizeInPixel_X.Width_U32, _rFrameData_X.FrameSizeInPixel_X.Height_U32, _rFrameData_X.NbChannel_U32, _rFrameData_X.FrameBuffer_X.pData_U8) == 0) ? BOF_ERR_INTERNAL : BOF_ERR_NO_ERROR;
      break;

    case BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_JPG:
      /*
      The last parameter of the stbi_write_jpg function is a quality parameter that goes from 1 to 100.
      Since JPG is a lossy image format, you can chose how much data is dropped at save time. Lower quality
      means smaller image size on disk and lower visual image quality.
      Most image editors, like GIMP, will save jpg images with a default quality parameter of 80 or 90,
      but the user can tune the quality parameter if required. I’ve used a quality parameter of 100 in all
      examples from this article.
      */
      Rts_E = (stbi_write_jpg(_rPath.FullPathName(false).c_str(), _rFrameData_X.FrameSizeInPixel_X.Width_U32, _rFrameData_X.FrameSizeInPixel_X.Height_U32, _rFrameData_X.NbChannel_U32, _rFrameData_X.FrameBuffer_X.pData_U8, _EncQuality_U32) == 0) ? BOF_ERR_INTERNAL : BOF_ERR_NO_ERROR;
      break;

    default:
      Rts_E = BOF_ERR_FORMAT;
      break;
    }
  }
  return Rts_E;
}


BOFERR Bof2d_SdlCheckIfError(int _SdlErrorCode_i, const std::string& _rErrorContext_S, const std::string& _rFile_S, const std::string& _rFunction_S, uint32_t _Line_U32)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  static const std::map<int, BOFERR> S_Sdl2BoferrorCodeCollection
  {
    {SDL_ENOMEM       , BOF_ERR_ENOMEM },
    {SDL_EFREAD       , BOF_ERR_READ},
    {SDL_EFWRITE      , BOF_ERR_WRITE },
    {SDL_EFSEEK       , BOF_ERR_SEEK},
    {SDL_UNSUPPORTED  , BOF_ERR_NOT_SUPPORTED},
    {SDL_LASTERROR    , BOF_ERR_INTERNAL},
  };
 
  if (_SdlErrorCode_i < 0)
  {
    auto It = S_Sdl2BoferrorCodeCollection.find(_SdlErrorCode_i);
    if (It != S_Sdl2BoferrorCodeCollection.end())
    {
      Rts_E = It->second;
    }
    else
    {
      Rts_E = BOF_ERR_INTERFACE;
      //Rts_E = (BOFERR)(_SdlErrorCode_i));
    }
    printf("%s:%s:%04d Error %d/%X\nBof:    %s\nCtx:    %s\nSdl: %s\n", _rFile_S.c_str(), _rFunction_S.c_str(), _Line_U32, _SdlErrorCode_i, _SdlErrorCode_i, BOF::Bof_ErrorCode(Rts_E), _rErrorContext_S.c_str(), SDL_GetError());
  }
  return Rts_E;
}
#define SDL_CHK_IF_ERR(Sts, Ctx, Rts)  {const char *pFile_c; BOF_GET_FILE_FROM__FILE__(pFile_c); Rts = Bof2d_SdlCheckIfError(Sts, Ctx, pFile_c, __func__, __LINE__);}

//https://gigi.nullneuron.net/gigilabs/displaying-an-image-in-an-sdl2-window/
BOF2D_EXPORT BOFERR Bof_ViewGraphicFile(const std::string &_rTitle_S, BOF_SIZE &_rWindowSize_X, const BOF_FRAME_DATA& _rFrameData_X)
{
BOFERR Rts_E = BOF_ERR_EINVAL;
bool Finish_B = false;
SDL_Event SdlEvent_E;
SDL_Window *pSdlWnd_X;
SDL_Renderer *pSdlRenderer;

  if ((_rFrameData_X.FrameBuffer_X.pData_U8) && (_rFrameData_X.FrameBuffer_X.Size_U64))
  {
    SDL_Init(SDL_INIT_VIDEO);
    pSdlWnd_X = SDL_CreateWindow(_rTitle_S.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _rWindowSize_X.Width_U32, _rWindowSize_X.Height_U32, 0);
    if (pSdlWnd_X == nullptr)
    {
      SDL_CHK_IF_ERR(SDL_ENOMEM, "Could not SDL_CreateWindow", Rts_E);
    }
    else
    {
      pSdlRenderer = SDL_CreateRenderer(pSdlWnd_X, -1, 0);
      if (pSdlWnd_X == nullptr)
      {
        SDL_CHK_IF_ERR(AVERROR_STREAM_NOT_FOUND, "Could not find decoder for codec " + std::to_string(mpVidDecCodecParam_X->codec_id), Rts_E);
      }
      else
      {
        while (!Finish_B)
        {
          SDL_WaitEvent(&SdlEvent_E);

          switch (SdlEvent_E.type)
          {
          case SDL_QUIT:
            Finish_B = true;
            break;
          }
        }
      }
    }
    SDL_Quit();

  }
  return Rts_E;
}

END_BOF2D_NAMESPACE()