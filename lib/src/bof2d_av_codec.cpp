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
#include <bof2d/bof2d_av_codec.h>
#include <bof2d/bof2d_video_decoder.h>
#include <bof2d/bof2d_audio_decoder.h>
#include <bof2d/bof2d_video_encoder.h>
#include <bof2d/bof2d_audio_encoder.h>

#include <map>

BEGIN_BOF2D_NAMESPACE()

BOF::BofEnum<BOF2D_AV_CONTAINER_FORMAT> S_Bof2dAvContainerFormatEnumConverter({
  { BOF2D_AV_CONTAINER_FORMAT::BOF2D_AV_CONTAINER_FORMAT_NONE, "NONE" },
  { BOF2D_AV_CONTAINER_FORMAT::BOF2D_AV_CONTAINER_FORMAT_MP4, "MP4" },
  { BOF2D_AV_CONTAINER_FORMAT::BOF2D_AV_CONTAINER_FORMAT_MAX, "MAX" },
  }, BOF2D_AV_CONTAINER_FORMAT::BOF2D_AV_CONTAINER_FORMAT_NONE);

BOF::BofEnum<BOF2D_AV_VIDEO_FORMAT> S_Bof2dAvVideoFormatEnumConverter({
  { BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_PNG, "PNG" },
  { BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_TGA, "TGA" },
  { BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_JPG, "JPG" },
  { BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_BMP, "BMP" },
  { BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_MAX, "MAX" },
  }, BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_MAX);

BOF::BofEnum<BOF2D_AV_AUDIO_FORMAT> S_Bof2dAvAudioFormatEnumConverter({
  { BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_PCM, "PCM" },
  { BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_WAV, "WAV" },
  { BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_MAX, "MAX" },
  }, BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_MAX);

BOF::BofEnum<BOF2D_AV_CODEC_STATE> S_Bof2dAvCodecStateEnumConverter({
  { BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE, "IDLE" },
  { BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_READY, "READY" },
  { BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_BUSY, "BUSY" },
  { BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_PENDING, "PENDING" },
  { BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_MAX, "MAX" },
  }, BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_MAX);

extern BOF::BofEnum<BOF2D_AV_CODEC_STATE> S_Bof2dAvCodecStateEnumConverter;

Bof2dAvCodec::Bof2dAvCodec(int _LogLevel_i)
{
  //AV_LOG_TRACE
  av_log_set_callback(Bof2d_FfmpegLogCallback);
  av_log_set_level(_LogLevel_i);  // AV_LOG_VERBOSE); // AV_LOG_TRACE); // AV_LOG_ERROR);
  printf("FFmpeg avformat version: %08X\nInfo: %s\nConfig: %s\nLicense: %s\n", avformat_version(), av_version_info(), avformat_configuration(), avformat_license());
  printf("FFmpeg avcodec version:  %08X\nInfo: %s\nConfig: %s\nLicense: %s\n", avcodec_version(), av_version_info(), avcodec_configuration(), avcodec_license());
  printf("FFmpeg avutil version:   %08X\nInfo: %s\nConfig: %s\nLicense: %s\n", avutil_version(), av_version_info(), avutil_configuration(), avutil_license());
  printf("FFmpeg swscale version:  %08X\nInfo: %s\nConfig: %s\nLicense: %s\n", swscale_version(), av_version_info(), swscale_configuration(), swscale_license());

  printf("LogLevel is %d\n", av_log_get_level());

  mpuVideoDecoder = std::make_unique<Bof2dVideoDecoder>();
  BOF_ASSERT(mpuVideoDecoder != nullptr);
  mpuAudioDecoder = std::make_unique<Bof2dAudioDecoder>();
  BOF_ASSERT(mpuAudioDecoder != nullptr);
  mpuVideoEncoder = std::make_unique<Bof2dVideoEncoder>();
  BOF_ASSERT(mpuVideoEncoder != nullptr);
  mpuAudioEncoder = std::make_unique<Bof2dAudioEncoder>();
  BOF_ASSERT(mpuAudioEncoder != nullptr);
}
Bof2dAvCodec::~Bof2dAvCodec()
{
  mpuAudioDecoder->Close();
  mpuVideoDecoder->Close();
  mpuAudioEncoder->Close();
  mpuVideoEncoder->Close();
}
BOFERR Bof2dAvCodec::OpenDecoder(const std::string &_rInputFile_S, const std::string &_rVidDecOption_S, const std::string &_rAudDecOption_S)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR, StsVideo_E, StsAudio_E;
  int Sts_i;

  // Open media file.
  Sts_i = avformat_open_input(&mpDecFormatCtx_X, _rInputFile_S.c_str(), nullptr, nullptr);
  FFMPEG_CHK_IF_ERR(Sts_i, "Couldn't open file " + _rInputFile_S, Rts_E);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    /* Retrieve stream information */
    Sts_i = avformat_find_stream_info(mpDecFormatCtx_X, nullptr);
    FFMPEG_CHK_IF_ERR(Sts_i, "Couldn't find stream information in " + _rInputFile_S, Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      StsVideo_E = mpuVideoDecoder->Open(mpDecFormatCtx_X, _rVidDecOption_S, mVidDecStreamIndex_i);

      StsAudio_E = mpuAudioDecoder->Open(mpDecFormatCtx_X, _rAudDecOption_S, mAudDecStreamIndex_i);

      if ((StsVideo_E != BOF_ERR_NO_ERROR) && (mpuVideoDecoder->IsVideoStreamPresent()))
      {
        Rts_E = BOF_ERR_NOT_OPENED;
      }
      else if ((StsAudio_E != BOF_ERR_NO_ERROR) && (mpuAudioDecoder->IsAudioStreamPresent()))
      {
        Rts_E = BOF_ERR_NOT_OPENED;
      }
    }
  }
  return Rts_E;
}
BOFERR Bof2dAvCodec::BeginRead(BOF2D_VID_DEC_OUT &_rVidDecData_X, BOF2D_AUD_DEC_OUT &_rAudDecData_X)
{
  BOFERR Rts_E;
  int Sts_i;
  bool GotNewData_B;

  _rAudDecData_X.Reset();
  _rVidDecData_X.Reset();

  do
  {
    GotNewData_B = false;
    Sts_i = av_read_frame(mpDecFormatCtx_X, &mDecPacket_X);
    FFMPEG_CHK_IF_ERR(Sts_i, "Cannot av_read_frame", Rts_E);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      if (mDecPacket_X.stream_index == mAudDecStreamIndex_i)
      {
        Rts_E = mpuAudioDecoder->BeginRead(&mDecPacket_X, _rAudDecData_X);
        GotNewData_B = (Rts_E == BOF_ERR_NO_ERROR);
      }
      else if (mDecPacket_X.stream_index == mVidDecStreamIndex_i)
      {
        Rts_E = mpuVideoDecoder->BeginRead(&mDecPacket_X, _rVidDecData_X);
        GotNewData_B = (Rts_E == BOF_ERR_NO_ERROR);
      }
      //else ? ? ?
      if (GotNewData_B)
      {
        break;
      }
    }
  } while (Rts_E == BOF_ERR_NO_ERROR);

  return Rts_E;
}
BOFERR Bof2dAvCodec::EndRead()
{
  BOFERR StsVideo_E = BOF_ERR_NO_ERROR, StsAudio_E = BOF_ERR_NO_ERROR;

  StsVideo_E = mpuVideoDecoder->IsVideoStreamPresent() ? mpuVideoDecoder->EndRead() : BOF_ERR_NO_ERROR;

  StsAudio_E = mpuAudioDecoder->IsAudioStreamPresent() ? mpuAudioDecoder->EndRead() : BOF_ERR_NO_ERROR;

  return (StsVideo_E == BOF_ERR_NO_ERROR) ? StsAudio_E : StsVideo_E;
}
BOFERR Bof2dAvCodec::CloseDecoder()
{
  BOFERR Rts_E;

  Rts_E = mpuVideoDecoder->Close();
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = mpuAudioDecoder->Close();
    avformat_close_input(&mpDecFormatCtx_X);
    mVidDecStreamIndex_i = -1;
    mAudDecStreamIndex_i = -1;
  }
  return Rts_E;
}
BOFERR Bof2dAvCodec::OpenEncoder(BOF2D_AV_CONTAINER_FORMAT _ContainerFormat_E, const std::string &_rVidEncOption_S, const std::string &_rAudEncOption_S)
{
  //_ContainerFormat_E can be none 
  BOFERR StsVideo_E, StsAudio_E;
  AVRational FrameRate_X = { 0, 0 };

  if (mpuVideoDecoder)
  {
    FrameRate_X = mpuVideoDecoder->GetVideoFrameRate();
  }

  StsVideo_E = mpuVideoEncoder->Open(_rVidEncOption_S, FrameRate_X);

  StsAudio_E = mpuAudioEncoder->Open(_rAudEncOption_S, FrameRate_X);

  return (StsVideo_E == BOF_ERR_NO_ERROR) ? StsAudio_E : StsVideo_E;
}
BOFERR Bof2dAvCodec::BeginWrite(BOF2D_VID_DEC_OUT &_rVidDecOut_X, BOF2D_AUD_DEC_OUT &_rAudDecOut_X)
{
  BOFERR StsVideo_E = BOF_ERR_NO_ERROR, StsAudio_E = BOF_ERR_NO_ERROR;

  StsVideo_E = (_rVidDecOut_X.Data_X.Size_U64) ? mpuVideoEncoder->BeginWrite(_rVidDecOut_X) : BOF_ERR_NO_ERROR;

  StsAudio_E = (_rAudDecOut_X.InterleavedData_X.Size_U64) ? mpuAudioEncoder->BeginWrite(_rAudDecOut_X) : BOF_ERR_NO_ERROR;

  return (StsVideo_E == BOF_ERR_NO_ERROR) ? StsAudio_E : StsVideo_E;
}
BOFERR Bof2dAvCodec::EndWrite()
{
  BOFERR StsVideo_E = BOF_ERR_NO_ERROR, StsAudio_E = BOF_ERR_NO_ERROR;

  StsVideo_E = mpuVideoEncoder->IsVideoStreamPresent() ? mpuVideoEncoder->EndWrite() : BOF_ERR_NO_ERROR;

  StsAudio_E = mpuAudioEncoder->IsAudioStreamPresent() ? mpuAudioEncoder->EndWrite() : BOF_ERR_NO_ERROR;

  return (StsVideo_E == BOF_ERR_NO_ERROR) ? StsAudio_E : StsVideo_E;
}

BOFERR Bof2dAvCodec::CloseEncoder()
{
  BOFERR Rts_E;

  Rts_E = mpuVideoEncoder->Close();
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    Rts_E = mpuAudioEncoder->Close();
    mVidEncStreamIndex_i = -1;
    mAudEncStreamIndex_i = -1;
  }
  return Rts_E;

}
/*
* @param avcl A pointer to an arbitrary struct of which the first field is a
 *        pointer to an AVClass struct or nullptr if general log.
 * @param level The importance level of the message expressed using a @ref
 *        lavu_log_constants "Logging Constant".
 * @param fmt The format string (printf-compatible) that specifies how
 *        subsequent arguments are converted to output.
*/
void Bof2d_FfmpegLogCallback(void *_pAvcl, int _Level_i, const char *_pFormat_c, va_list _VaList)
{
  char pLog_c[0x1000];
  //va_list VaList;
  if (_Level_i <= av_log_get_level())
  {
    //av_printf_format(3, 4);
        //va_start(VaList, _pFormat_c);
    vsprintf(pLog_c, _pFormat_c, _VaList);
    //va_end(VaList);
    printf("Ffmpeg L%03d->%s", _Level_i, pLog_c);
  }
}


END_BOF2D_NAMESPACE()