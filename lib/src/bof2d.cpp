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
/*
Republish a new version
- Change version in C:\pro\github\bof2d\CMakeLists.txt: project(bof2d VERSION 4.1.1.4)
- Change version in C:\pro\github\bof2d\vcpkg.json:     "version": "4.1.1.4",
- Change version in C:\pro\github\bof2d\vcpkg-configuration.json 
                BofStd depend "baseline": "8c8de87e6b5b6fcfe568026faa0bf9f12767dbbc",
Push all modified files to git and get git push ref: 
get push ref 05ea40bc619927fb9f664ad75af2c60792e8bc45		used also during last phase with the .\update-port.py command at rhe end of the procedure

- Change REF and SHA512 in C:\pro\github\onbings-vcpkg-registry\ports\bof2d\portfile.cmake 
  REF 05ea40bc619927fb9f664ad75af2c60792e8bc45
  SHA512 0

- Validate BofSrd and Bof2d with the C:\pro\github\vcpkg-packaging-env project
  cd C:\bld\b
  del * /S /Q
  cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\github\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-dynamic -DVCPKG_OVERLAY_PORTS=C:\pro\github\onbings-vcpkg-registry\ports\ C:\pro\github\vcpkg-packaging-env

  [DEBUG] Downloading https:\\github.com\onbings\bofstd\archive\05ea40bc619927fb9f664ad75af2c60792e8bc45.tar.gz
  Error: Failed to download from mirror set:
  File does not have the expected hash:
             url : [ https:\\github.com\onbings\bofstd\archive\05ea40bc619927fb9f664ad75af2c60792e8bc45.tar.gz ]
       File path : [ C:\pro\vcpkg\downloads\onbings-bofstd-05ea40bc619927fb9f664ad75af2c60792e8bc45.tar.gz.20964.part ]
   Expected hash : [ 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 ]
     Actual hash : [ cc7ce519399c6e77f0404900b98faf43c4b52143dd504377360b339768dfaf46e031c5b40d830f40fc3c3ca2339b1ddba7d4ba9ac0bc584f4c84e96d9fae7867 ]

  get the Actual hash  and put it back in SHA512 in C:\pro\github\onbings-vcpkg-registry\ports\bof2d\portfile.cmake 

  cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\github\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-dynamic -DVCPKG_OVERLAY_PORTS=C:\pro\github\onbings-vcpkg-registry\ports\ C:\pro\github\vcpkg-packaging-env
  Should be ok, compile it with  cmake --build .

  cd C:\pro\github\onbings-vcpkg-registry
  git pull
  python .\update-port.py bof2d 4.1.1.4 05ea40bc619927fb9f664ad75af2c60792e8bc45
  git push
  git log
  commit message should be "Update bofstd to 4.1.1.4\05ea40bc619927fb9f664ad75af2c60792e8bc45"

  get the new push git ref 4eb2199109841185f5075ecad7deed414f2c4f55

  update baseline in your vcpkg-configuration of your project which want to use this new lib C:\pro\github\vcpkg-packaging-env\vcpkg-configuration.json

  "baseline": "4eb2199109841185f5075ecad7deed414f2c4f55",

DLL:
doxygen:
cmake -DEVS_MUSE_STORAGE_GENERATE_HELP=ON -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-dynamic C:\pro\github\vcpkg-packaging-env
cmake -DEVS_MUSE_STORAGE_GENERATE_HELP=ON -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-static C:\pro\github\vcpkg-packaging-env

cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=ON -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-dynamic C:\pro\github\vcpkg-packaging-env
LIB:
cmake -DCMAKE_TOOLCHAIN_FILE=C:\pro\vcpkg\scripts\buildsystems\vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DVCPKG_TARGET_TRIPLET=evs-x64-swx-windows-static C:\pro\github\vcpkg-packaging-env
*/

#include <bof2d/bof2d.h>
#include <bof2d_version_info.h>

#include <zlib.h>
#include <SDL.h>        

#include <map>
extern "C"
{
#include <libavcodec/avcodec.h>
}

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

uint8_t *ZlibCompressor(unsigned char *data, int _DataToCompressSizeInByte_i, int *out_len, int quality);
#define STBIW_ZLIB_COMPRESS ZlibCompressor

/*
   You can #define STBIW_ZLIB_COMPRESS to use a custom zlib-style compress function
   for PNG compression (instead of the builtin one), it must have the following signature:
   unsigned char * my_compress(unsigned char *data, int _DataToCompressSizeInByte_i, int *out_len, int quality);
   The returned data will be freed with STBIW_FREE() (free() by default),
   so it must be heap allocated with STBIW_MALLOC() (malloc() by default),

*/
//const long int CHUNK = { 16384 };
uint8_t *ZlibCompressor(uint8_t *_pDataToCompress_U8, int _DataToCompressSizeInByte_i, int *_pDataCompressedSizeInByte_i, int _Quality_i)
{
  uint8_t *pRts_U8 = nullptr;
  unsigned long OutLen_U32 = 0;
  int Sts_i;

  if ((_pDataToCompress_U8) && (_pDataCompressedSizeInByte_i))
  {
    pRts_U8 = (uint8_t *)STBIW_MALLOC(_DataToCompressSizeInByte_i);
    OutLen_U32 = _DataToCompressSizeInByte_i;
    Sts_i = compress2(pRts_U8, &OutLen_U32, _pDataToCompress_U8, _DataToCompressSizeInByte_i, _Quality_i);
    if (Sts_i)
    {
      STBIW_FREE(pRts_U8);
      pRts_U8 = nullptr;
      OutLen_U32 = 0;
    }
  }
  *_pDataCompressedSizeInByte_i = static_cast<int>(OutLen_U32);

  return pRts_U8;
}

BOF2D_EXPORT BOF2D::BOF2DPARAM GL_Bof2dParam_X;

BEGIN_BOF2D_NAMESPACE()

struct BOF2D_STATE
{
  bool LibInit_B;
  ma_engine *pAudioEngine_X;
  BOF2D_STATE()
  {
    Reset();
  }
  void Reset()
  {
    LibInit_B = false;
    pAudioEngine_X = nullptr;
  }
};
static BOF2D_STATE S_Bof2dState_X;

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

BOFERR Bof2d_SdlCheckIfError(int _SdlErrorCode_i, const std::string &_rErrorContext_S, const std::string &_rFile_S, const std::string &_rFunction_S, uint32_t _Line_U32)
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

  if (_SdlErrorCode_i)
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
    printf("%s:%s:%04d Error %d/%X\nBof: %s\nCtx: %s\nSdl: %s\n", _rFile_S.c_str(), _rFunction_S.c_str(), _Line_U32, _SdlErrorCode_i, _SdlErrorCode_i, BOF::Bof_ErrorCode(Rts_E), _rErrorContext_S.c_str(), SDL_GetError());
  }
  return Rts_E;
}

BOFERR Bof2d_FfmpegCheckIfError(int _FfmpegErrorCode_i, const std::string &_rErrorContext_S, const std::string &_rFile_S, const std::string &_rFunction_S, uint32_t _Line_U32)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  std::string Err_S;
  size_t ErrorSize = AV_ERROR_MAX_STRING_SIZE;
  static const std::map<int, BOFERR> S_Ffmpeg2BoferrorCodeCollection
  {
    {AVERROR_BSF_NOT_FOUND     , BOF_ERR_NOT_FOUND },
    {AVERROR_BUG               , BOF_ERR_INTERNAL},
    {AVERROR_BUFFER_TOO_SMALL  , BOF_ERR_TOO_SMALL},
    {AVERROR_DECODER_NOT_FOUND , BOF_ERR_CODEC},
    {AVERROR_DEMUXER_NOT_FOUND , BOF_ERR_INPUT},
    {AVERROR_ENCODER_NOT_FOUND , BOF_ERR_OUTPUT},
    {AVERROR_EOF               , BOF_ERR_EOF},
    {AVERROR_EXIT              , BOF_ERR_CANNOT_STOP},
    {AVERROR_EXTERNAL          , BOF_ERR_ELIBACC},
    {AVERROR_FILTER_NOT_FOUND  , BOF_ERR_NOT_FOUND},
    {AVERROR_INVALIDDATA       , BOF_ERR_ENODATA},
    {AVERROR_MUXER_NOT_FOUND   , BOF_ERR_NOT_FOUND},
    {AVERROR_OPTION_NOT_FOUND  , BOF_ERR_NOT_FOUND},
    {AVERROR_PATCHWELCOME      , BOF_ERR_EMLINK},
    {AVERROR_PROTOCOL_NOT_FOUND, BOF_ERR_NOT_FOUND},
    {AVERROR_STREAM_NOT_FOUND  , BOF_ERR_NOT_FOUND},
    {AVERROR_BUG2              , BOF_ERR_INTERNAL},
    {AVERROR_UNKNOWN           , BOF_ERR_INTERNAL},
    {AVERROR_EXPERIMENTAL      , BOF_ERR_INTERNAL},
    {AVERROR_INPUT_CHANGED     , BOF_ERR_INPUT},
    {AVERROR_OUTPUT_CHANGED    , BOF_ERR_OUTPUT},
    {AVERROR_HTTP_BAD_REQUEST  , BOF_ERR_EBADRQC},
    {AVERROR_HTTP_UNAUTHORIZED , BOF_ERR_EACCES},
    {AVERROR_HTTP_FORBIDDEN    , BOF_ERR_LOCK},
    {AVERROR_HTTP_NOT_FOUND    , BOF_ERR_NOT_FOUND},
    {AVERROR_HTTP_OTHER_4XX    , BOF_ERR_NOT_FOUND},
    {AVERROR_HTTP_SERVER_ERROR , BOF_ERR_ENODEV}
  };
  if (_FfmpegErrorCode_i < 0)
  {
    Err_S.resize(ErrorSize);
    if (av_strerror(_FfmpegErrorCode_i, &Err_S.at(0), ErrorSize))
    {
      Err_S = "Unable to translate Ffmpeg error code " + std::to_string(_FfmpegErrorCode_i);  //Less than AV_ERROR_MAX_STRING_SIZE 
    }

    auto It = S_Ffmpeg2BoferrorCodeCollection.find(_FfmpegErrorCode_i);
    if (It != S_Ffmpeg2BoferrorCodeCollection.end())
    {
      Rts_E = It->second;
    }
    else
    {
      //Rts_E = BOF_ERR_INTERFACE;
      Rts_E = (BOFERR)(AVUNERROR(_FfmpegErrorCode_i));
    }
    printf("%s:%s:%04d Error %d/%X\nBof:    %s\nCtx:    %s\nFfmpeg: %s\n", _rFile_S.c_str(), _rFunction_S.c_str(), _Line_U32, _FfmpegErrorCode_i, _FfmpegErrorCode_i, BOF::Bof_ErrorCode(Rts_E), _rErrorContext_S.c_str(), Err_S.c_str());
  }
  return Rts_E;
}
std::string Bof_GetVersion()
{
  //Just to check
  //const char *pFfmpegVersion_c = av_version_info();
  //const char *pOpenSslVersion_c = OpenSSL_version(OPENSSL_VERSION); 
  //const char *pBoostVersion_c = BOOST_LIB_VERSION;

  return std::to_string(BOF2D_VERSION_MAJOR) + "." + std::to_string(BOF2D_VERSION_MINOR) + "." + std::to_string(BOF2D_VERSION_PATCH) + "." + std::to_string(BOF2D_VERSION_BUILD);
}

BOFERR Bof_Initialize(const BOF2DPARAM &_r2dParam_X)
{
  BOFERR Rts_E = BOF_ERR_INIT;

  if (!S_Bof2dState_X.LibInit_B)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    GL_Bof2dParam_X = _r2dParam_X;
    SDL_Init(SDL_INIT_VIDEO);
    S_Bof2dState_X.LibInit_B = true;
  }
  return Rts_E;
}
BOFERR Bof_Shutdown()
{
  BOFERR Rts_E = BOF_ERR_INIT;

  if (S_Bof2dState_X.LibInit_B)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    SDL_Quit();
    S_Bof2dState_X.LibInit_B = false;
  }
  return Rts_E;
}
BOFERR Bof_ProcessEvent()
{
  BOFERR Rts_E = BOF_ERR_INIT;
  SDL_Event     SdlEvent_X;

  if (S_Bof2dState_X.LibInit_B)
  {
    Rts_E = BOF_ERR_EINVAL;
    //if (_pFinish_B)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      SDL_WaitEvent(&SdlEvent_X);
      //printf("SdlEvent_E[%d] %d\n", Loop_U32, SdlEvent_X.type);
      switch (SdlEvent_X.type)
      {
        case SDL_QUIT:
          Rts_E = BOF_ERR_FINISHED;
          break;

        case SDL_WINDOWEVENT:
          //printf("   SDL_WINDOWEVENT %d\n", SdlEvent_X.window.event);
          switch (SdlEvent_X.window.event)
          {
            //Get new dimensions and repaint on window size change
            case SDL_WINDOWEVENT_SIZE_CHANGED:
              //Width_i = SdlEvent_X.window.data1;
              //Height_i = SdlEvent_X.window.data2;
              //SDL_RenderPresent(pSdlRenderer_X);
              break;

              //Repaint on exposure
            case SDL_WINDOWEVENT_EXPOSED:
              //SDL_RenderPresent(pSdlRenderer_X);
              break;
              //Mouse entered window
            case SDL_WINDOWEVENT_ENTER:
              break;

              //Mouse left window
            case SDL_WINDOWEVENT_LEAVE:
              break;

              //Window has keyboard focus
            case SDL_WINDOWEVENT_FOCUS_GAINED:
              break;

              //Window lost keyboard focus
            case SDL_WINDOWEVENT_FOCUS_LOST:
              break;

              //Window minimized
            case SDL_WINDOWEVENT_MINIMIZED:
              break;

              //Window maximized
            case SDL_WINDOWEVENT_MAXIMIZED:
              break;

              //Window restored
            case SDL_WINDOWEVENT_RESTORED:
              break;
          }
      }
    }
  }
  return Rts_E;
}
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