/*
   Copyright (c) 2000-2026, OnBings. All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   This module defines the interface to the Ffmpeg lib

   Author:      Bernard HARMEL: onbings@gmail.com
   Web:			    onbings.dscloud.me
   Revision:    1.0

   History:

   V 1.00  Sep 30 2000  BHA : Initial release
 */

#pragma once
#include <bofstd/bofenum.h>
#include <bofstd/bofpath.h>
#include <bofstd/bofsystem.h>

#include <bof2d/bof2d.h>

extern "C"
{
#include <libavutil/common.h>
#include <libavutil/error.h>
#include <libavformat/avformat.h>
#include "libavutil/avutil.h"
}


BEGIN_BOF2D_NAMESPACE()
class Bof2dVideoDecoder;
class Bof2dAudioDecoder;
class Bof2dVideoEncoder;
class Bof2dAudioEncoder;

void Bof2d_FfmpegLogCallback(void *_pAvcl, int _Level_i, const char *_pFormat_c, va_list _VaList);

enum class BOF2D_AV_CONTAINER_FORMAT :int32_t
{
  BOF2D_AV_CONTAINER_FORMAT_NONE = 0,
  BOF2D_AV_CONTAINER_FORMAT_MP4,
  BOF2D_AV_CONTAINER_FORMAT_MAX
};
extern BOF::BofEnum<BOF2D_AV_CONTAINER_FORMAT> S_Bof2dAvContainerFormatEnumConverter;

enum class BOF2D_AV_VIDEO_FORMAT :int32_t
{
  BOF2D_AV_VIDEO_FORMAT_PNG = 0,
  BOF2D_AV_VIDEO_FORMAT_TGA,
  BOF2D_AV_VIDEO_FORMAT_JPG,
  BOF2D_AV_VIDEO_FORMAT_BMP,
  BOF2D_AV_VIDEO_FORMAT_MAX
};
extern BOF::BofEnum<BOF2D_AV_VIDEO_FORMAT> S_Bof2dAvVideoFormatEnumConverter;

enum class BOF2D_AV_AUDIO_FORMAT :int32_t
{
  BOF2D_AV_AUDIO_FORMAT_PCM = 0,
  BOF2D_AV_AUDIO_FORMAT_WAV,
  BOF2D_AV_AUDIO_FORMAT_MAX
};
extern BOF::BofEnum<BOF2D_AV_AUDIO_FORMAT> S_Bof2dAvAudioFormatEnumConverter;

enum class BOF2D_AV_CODEC_STATE :int32_t
{
  BOF2D_AV_CODEC_STATE_IDLE = 0,
  BOF2D_AV_CODEC_STATE_READY,
  BOF2D_AV_CODEC_STATE_BUSY,
  BOF2D_AV_CODEC_STATE_PENDING,
  BOF2D_AV_CODEC_STATE_MAX
};
extern BOF::BofEnum<BOF2D_AV_CODEC_STATE> S_Bof2dAvCodecStateEnumConverter;

struct BOF2D_EXPORT BOF2D_VID_DEC_OUT
{
  BOF::BOF_BUFFER Data_X;
  BOF2D::BOF_SIZE Size_X;
  int32_t LineSize_S32;
  AVPixelFormat PixelFmt_E;
  uint32_t NbChannel_U32; //3 RGB 4 RGBA
  AVRational FrameRate_X;

  BOF2D_VID_DEC_OUT()
  {
    Reset();
  }

  void Reset()
  {
    Data_X.Reset();
    Size_X.Reset();
    LineSize_S32 = 0;
    PixelFmt_E = AV_PIX_FMT_NONE;
    NbChannel_U32 = 0;
    FrameRate_X = { 0, 0 };
  }
};

struct BOF2D_EXPORT BOF2D_AUD_DEC_OUT
{
  uint32_t NbSample_U32;
  uint32_t NbChannel_U32;
  uint64_t ChannelLayout_U64;
  uint32_t SampleRateInHz_U32;
  uint32_t NbBitPerSample_U32;

  BOF::BOF_BUFFER InterleavedData_X;    //Global audio data
  std::vector<BOF::BOF_BUFFER> ChannelBufferCollection;   //Audio data demuxed per channel

  BOF2D_AUD_DEC_OUT()
  {
    Reset();
  }
  void Reset()
  {
    NbSample_U32 = 0;
    NbChannel_U32 = 0;
    ChannelLayout_U64 = 0;
    SampleRateInHz_U32 = 0;
    NbBitPerSample_U32 = 0;

    InterleavedData_X.Reset();
    ChannelBufferCollection.clear();
  }
};

class BOF2D_EXPORT Bof2dAvCodec
{
public:
  Bof2dAvCodec(int _LogLevel_i = AV_LOG_ERROR);
  virtual ~Bof2dAvCodec();

  BOFERR OpenDecoder(const std::string &_rInputFile_S, const std::string &_rVidDecOption_S, const std::string &_rAudDecOption_S);
  BOFERR OpenEncoder(BOF2D_AV_CONTAINER_FORMAT _ContainerFormat_E, const std::string &_rVidDecOption_S, const std::string &_rAudDecOption_S);
  BOFERR BeginRead(BOF2D_VID_DEC_OUT &_rVidDecOut_X, BOF2D_AUD_DEC_OUT &_rAudDecOut_X);
  BOFERR EndRead();
  BOFERR BeginWrite(BOF2D_VID_DEC_OUT &_rVidDecOut_X, BOF2D_AUD_DEC_OUT &_rAudDecOut_X);
  BOFERR EndWrite();
  BOFERR CloseEncoder();
  BOFERR CloseDecoder();

private:
  std::unique_ptr<Bof2dVideoDecoder> mpuVideoDecoder;
  std::unique_ptr<Bof2dAudioDecoder> mpuAudioDecoder;
  std::unique_ptr<Bof2dVideoEncoder> mpuVideoEncoder;
  std::unique_ptr<Bof2dAudioEncoder> mpuAudioEncoder;
  AVFormatContext *mpDecFormatCtx_X = nullptr;
  AVPacket mDecPacket_X;
  int mVidDecStreamIndex_i = -1;
  int mAudDecStreamIndex_i = -1;
  int mVidEncStreamIndex_i = -1;
  int mAudEncStreamIndex_i = -1;

};

END_BOF2D_NAMESPACE()
