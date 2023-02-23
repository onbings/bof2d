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

#include <SDL2/SDL.h>


 //Playing audio
static Uint8 *audio_chunk;
static Uint32 audio_len;
static Uint8 *audio_pos;

/* The audio function callback takes the following parameters:
   stream:  A pointer to the audio buffer to be filled
   len:     The length (in bytes) of the audio buffer
*/
void fill_audio(void *udata, Uint8 *stream, int len)
{
  /* Only play if we have data left */
  if (audio_len == 0)
    return;

  /* Mix as much data as possible */
  len = (len > audio_len ? audio_len : len);
  SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
  audio_pos += len;
  audio_len -= len;
}

TEST(Bof2d_ffmpeg_Test, sdl)
{
  SDL_AudioSpec wanted;
  extern void fill_audio(void *udata, Uint8 * stream, int len);

  /* Set the audio format */
  wanted.freq = 48000;
  wanted.format = AUDIO_S16;
  wanted.channels = 2;    /* 1 = mono, 2 = stereo */
  wanted.samples = 1024;  /* Good low-latency value for callback */
  wanted.callback = fill_audio;
  wanted.userdata = NULL;

  /* Open the audio device, forcing the desired format */
  if (SDL_OpenAudio(&wanted, NULL) < 0)
  {
    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    //return(-1);
  }
  //return(0);
 

  /* Load the audio data ... */

  //BOF::BOF_ReadFi

  audio_pos = audio_chunk;

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