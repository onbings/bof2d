/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bof2d/ffmpeg class
 *
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include "gtestrunner.h"
#include <bof2d/bof2d.h>

#include <bof2d/bof2d_av_codec.h>


#if 0     //SDL AUDIO

#include <SDL2/SDL.h>
#include <bofstd/boffs.h>


 //Playing audio
//static uint8_t *audio_chunk;
static uint32_t audio_len;
static BOF::BOF_BUFFER S_AudioBuffer_X;
static uint8_t *audio_pos;
constexpr uint32_t AUDIO_BUFFER_CHUNK = 0x1000;

/* The audio function callback takes the following parameters:
   stream:  A pointer to the audio buffer to be filled
   len:     The length (in bytes) of the audio buffer
*/
void fill_audio(void *udata, uint8_t *stream, int len)
{
  /* Only play if we have data left */
  if (audio_len == 0)
  {
    S_AudioBuffer_X  use new bof buffer seek method
  }
    return;

  /* Mix as much data as possible */
  len = (len > audio_len ? audio_len : len);
  SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
  audio_pos += len;
  audio_len -= len;
}

BOFERR Bof_24sleTo32sle(const BOF::BOF_BUFFER &_rSrcBuffer_X, BOF::BOF_BUFFER &_rDstBuffer_X)
{
  uint32_t i_U32, NbData24_U32;
  uint8_t *pDst_U8;
  const uint8_t *pSrc_U8;
  BOFERR Rts_E = BOF_ERR_TOO_BIG;

  if ((_rSrcBuffer_X.pData_U8) && (_rDstBuffer_X.pData_U8) && (((_rSrcBuffer_X.Size_U64 * 4) / 3) <= _rDstBuffer_X.Size_U64))
  {
    Rts_E = BOF_ERR_TOO_BIG;
    if ((_rSrcBuffer_X.Size_U64 % 12) == 0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      NbData24_U32 = _rSrcBuffer_X.Size_U64 / 12;
      pSrc_U8 = reinterpret_cast<const uint8_t *>(_rSrcBuffer_X.pData_U8);
      pDst_U8 = _rDstBuffer_X.pData_U8;
      for (i_U32 = 0; i_U32 < NbData24_U32; i_U32, pSrc_U8 += 3, pDst_U8 += 4)
      {
        pDst_U8[0] = pSrc_U8[0];
        pDst_U8[1] = pSrc_U8[1];
        pDst_U8[2] = pSrc_U8[2];
        pDst_U8[3] = (pSrc_U8[2] & 0x80) ? 0xFF : 0x00;
      }
    }
  }
  return Rts_E;
}

BOFERR Bof_32sleTo24sle(const BOF::BOF_BUFFER &_rSrcBuffer_X, BOF::BOF_BUFFER &_rDstBuffer_X)
{
  uint32_t i_U32, NbData32_U32;
  uint8_t *pDst_U8;
  const uint8_t *pSrc_U8;
  BOFERR Rts_E = BOF_ERR_TOO_BIG;

  if ((_rSrcBuffer_X.pData_U8) && (_rDstBuffer_X.pData_U8) && (((_rSrcBuffer_X.Size_U64 * 3) / 4) <= _rDstBuffer_X.Size_U64))
  {
    Rts_E = BOF_ERR_TOO_BIG;
    if ((_rSrcBuffer_X.Size_U64 % 12) == 0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      NbData32_U32 = _rSrcBuffer_X.Size_U64 / 4;
      pSrc_U8 = reinterpret_cast<const uint8_t *>(_rSrcBuffer_X.pData_U8);
      pDst_U8 = _rDstBuffer_X.pData_U8;
      for (i_U32 = 0; i_U32 < NbData32_U32; i_U32, pSrc_U8 += 4, pDst_U8 += 3)
      {
        pDst_U8[0] = pSrc_U8[0];
        pDst_U8[1] = pSrc_U8[1];
        if (pSrc_U8[3] & 0x80)
        {
          pDst_U8[2] = pSrc_U8[2] | 0x80;
        }
        else
        {
          pDst_U8[2] = pSrc_U8[2] & 0x7F;
        }
      }
    }
  }
  return Rts_E;
}

TEST(Bof2d_ffmpeg_Test, sdl)
{
  BOFERR Sts_E;
  BOF::BOF_BUFFER BufferToDeleteAfterUsage_X, SrcBuffer_X;
  uint32_t AudioChunkSize_U32;
  SDL_AudioSpec SdlAudioContext_X;
  int Sts_i;

  extern void fill_audio(void *udata, uint8_t * stream, int len);

  /* Set the audio format */
  SdlAudioContext_X.freq = 48000;
  SdlAudioContext_X.format = AUDIO_S32LSB;
  SdlAudioContext_X.channels = 2;    /* 1 = mono, 2 = stereo */
  SdlAudioContext_X.samples = AUDIO_BUFFER_CHUNK;  /* Good low-latency value for callback */
  SdlAudioContext_X.callback = fill_audio;
  SdlAudioContext_X.userdata = nullptr;

  /* Open the audio device, forcing the desired format */
  Sts_i = SDL_OpenAudio(&SdlAudioContext_X, nullptr);
  SDL_CHK_IF_ERR(Sts_i, "Unable to SDL_OpenAudio", Sts_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  /* Load the audio data ... */
  EXPECT_EQ(BOF::Bof_ReadFile("C:/tmp/ffmpeg/AudioOut.PCM", BufferToDeleteAfterUsage_X), BOF_ERR_NO_ERROR);
  SrcBuffer_X.SetStorage(BufferToDeleteAfterUsage_X.Size_U64 * 4 / 3, BufferToDeleteAfterUsage_X.Size_U64 * 4 / 3, nullptr);
  EXPECT_EQ(Bof_24sleTo32sle(BufferToDeleteAfterUsage_X, SrcBuffer_X), BOF_ERR_NO_ERROR);
  AudioChunkSize_U32 = (SrcBuffer_X.Size_U64 < AUDIO_BUFFER_CHUNK) ? static_cast<uint32_t>(SrcBuffer_X.Size_U64) : AUDIO_BUFFER_CHUNK;
  S_AudioBuffer_X.SetStorage(AudioChunkSize_U32, AudioChunkSize_U32, SrcBuffer_X.pData_U8);

  audio_pos = SrcBuffer_X.pData_U8;

  /* Let the callback function play the audio chunk */
  SDL_PauseAudio(0);

  /* Do some processing */

  ;;;;;

  /* Wait for sound to complete */
  while (audio_len > 0)
  {
    SDL_Delay(100);         /* Sleep 1/10 second */
  }
  SDL_CloseAudio();
}

#endif

TEST(Bof2d_ffmpeg_Test, Api)
{
  BOF2D::BOF2D_VID_DEC_OUT VidDecOut_X;
  BOF2D::BOF2D_AUD_DEC_OUT AudDecOut_X;
  uint64_t VideoFrameCounter_U64, AudioFrameCounter_U64;
  int16_t *pAudioData_S16;
  uint32_t *pVideoData_U32;
  bool EncodeData_B;
  BOF2D::Bof2dAvCodec AvCodec;

  //First decoder before encoder !!!
#if 0
  EXPECT_EQ(AvCodec.OpenDecoder("./data/Sample24bit96kHz5.1.wav", "", "--A_NBCHNL=6;--A_LAYOUT=0x3F;--A_RATE=48000;--A_BPS=16;--A_DEMUX"), BOF_ERR_NO_ERROR);
#else
  EXPECT_EQ(AvCodec.OpenDecoder("./data/sample-mp4-file.mp4", "--V_WIDTH=160;--V_HEIGHT=120;--V_BPS=32;--V_THREAD=1",
    "--A_NBCHNL=2;--A_LAYOUT=3;--A_RATE=48000;--A_BPS=24;--A_DEMUX;--A_THREAD=1"), BOF_ERR_NO_ERROR);
#endif
  EXPECT_EQ(AvCodec.OpenEncoder(BOF2D::BOF2D_AV_CONTAINER_FORMAT::BOF2D_AV_CONTAINER_FORMAT_NONE, "--V_BASEFN=C:/tmp/ffmpeg/VideoOut;--V_FMT=PNG;--V_QUALITY=90",
    "--A_BASEFN=C:/tmp/ffmpeg/AudioOut;--A_NBCHNL=2;--A_FMT=PCM;--A_CHUNKSIZE= 801, 801, 801, 801, 800"), BOF_ERR_NO_ERROR);
  VideoFrameCounter_U64 = 0;
  AudioFrameCounter_U64 = 0;
  while (1)
  {
    EncodeData_B = false;
    EXPECT_EQ(AvCodec.BeginRead(VidDecOut_X, AudDecOut_X), BOF_ERR_NO_ERROR);

    if (VidDecOut_X.Data_X.Size_U64)
    {
      VideoFrameCounter_U64++;
      EncodeData_B = true;

      pVideoData_U32 = (uint32_t *)VidDecOut_X.Data_X.pData_U8;
      BOF_ASSERT(pVideoData_U32 != nullptr);
      if (pVideoData_U32)
      {
        printf("%06zd: Got Video %zx/%zx:%p Ls %d %dx%d\n", VideoFrameCounter_U64, VidDecOut_X.Data_X.Size_U64, VidDecOut_X.Data_X.Capacity_U64, VidDecOut_X.Data_X.pData_U8,
          VidDecOut_X.LineSize_S32, VidDecOut_X.Size_X.Width_U32, VidDecOut_X.Size_X.Height_U32);
        printf("        Got Data %08x %08x %08x %08x %08x %08x %08x %08x\n", pVideoData_U32[0], pVideoData_U32[1], pVideoData_U32[2], pVideoData_U32[3], pVideoData_U32[4], pVideoData_U32[5], pVideoData_U32[6], pVideoData_U32[7]);
      }
    }
    if (AudDecOut_X.InterleavedData_X.Size_U64)
    {
      AudioFrameCounter_U64++;
      EncodeData_B = true;

      pAudioData_S16 = (int16_t *)AudDecOut_X.InterleavedData_X.pData_U8;
      BOF_ASSERT(pAudioData_S16 != nullptr);
      if (pAudioData_S16)
      {
        printf("%06zd: Got Audio %zx/%zx:%p nbs %d ch %d layout %zx Rate %d\n", AudioFrameCounter_U64, AudDecOut_X.InterleavedData_X.Size_U64, AudDecOut_X.InterleavedData_X.Capacity_U64, AudDecOut_X.InterleavedData_X.pData_U8,
          AudDecOut_X.NbSample_U32, AudDecOut_X.NbChannel_U32, AudDecOut_X.ChannelLayout_U64, AudDecOut_X.SampleRateInHz_U32);
        printf("        Got Data %04x %04x %04x %04x %04x %04x %04x %04x\n", pAudioData_S16[0], pAudioData_S16[1], pAudioData_S16[2], pAudioData_S16[3], pAudioData_S16[4], pAudioData_S16[5], pAudioData_S16[6], pAudioData_S16[7]);
      }
    }
    if (EncodeData_B)
    {
      EXPECT_EQ(AvCodec.BeginWrite(VidDecOut_X, AudDecOut_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(AvCodec.EndWrite(), BOF_ERR_NO_ERROR);
    }
    EXPECT_EQ(AvCodec.EndRead(), BOF_ERR_NO_ERROR);

    //if ((VideoFrameCounter_U64 > 512))	// || (AudioFrameCounter_U64 > 512))
    if ((VideoFrameCounter_U64 > 100))	// || (AudioFrameCounter_U64 > 512))
    {
      break;
    }
  }
  EXPECT_EQ(AvCodec.CloseEncoder(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(AvCodec.CloseDecoder(), BOF_ERR_NO_ERROR);
}