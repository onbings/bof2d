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
#include <bof2d/bof2d_convert.h>
#include <bof2d/bof2d_av_codec.h>


#if 1     //SDL AUDIO

#include <SDL2/SDL.h>
#include <bofstd/boffs.h>

constexpr uint32_t AUDIO_BUFFER_CHUNK_SIZE_IN_SAMPLE_FOR_ALL_CHANNEL = 0x100;

void Sdl_FillAudioBuffer_Callback(void *_pUser, uint8_t *_pAudioBufferToFill_U8, int _LenOfAudioBuffer_i)
{
  int Len_i;
  uint8_t *pAudioData_U8;
  uint64_t NbRead_U64, Remain_U64;
  BOF::BOF_BUFFER *pFullAudioBuffer_X = reinterpret_cast<BOF::BOF_BUFFER *>(_pUser);

  if (pFullAudioBuffer_X)
  {
    Remain_U64 = pFullAudioBuffer_X->RemainToRead();
    if (Remain_U64)
    {
      Len_i = (Remain_U64 < _LenOfAudioBuffer_i) ? Remain_U64 : _LenOfAudioBuffer_i;
      pAudioData_U8 = pFullAudioBuffer_X->Read(Len_i, NbRead_U64);
      SDL_MixAudio(_pAudioBufferToFill_U8, pAudioData_U8, Len_i, SDL_MIX_MAXVOLUME);
    }
    printf("pAudioBufferToFill %x:%p Rem %zx Sz %zx Off %zx pAudioData %p\n", Len_i, _pAudioBufferToFill_U8, Remain_U64, pFullAudioBuffer_X->Size_U64, pFullAudioBuffer_X->Offset_U64, pAudioData_U8);
  }
}

TEST(Bof2d_ffmpeg_Test, sdl)
{
  BOFERR Sts_E;
  BOF::BOF_BUFFER DiskAudioBuffer_X, FullAudioBuffer_X;
  SDL_AudioSpec SdlAudioSpec_X;
  int Sts_i;

  SdlAudioSpec_X.freq = 48000;
  SdlAudioSpec_X.format = AUDIO_S32LSB;
  SdlAudioSpec_X.channels = 1;  // 2;    /* 1 = mono, 2 = stereo */
  SdlAudioSpec_X.samples = AUDIO_BUFFER_CHUNK_SIZE_IN_SAMPLE_FOR_ALL_CHANNEL;  /* Good low-latency value for callback */
  SdlAudioSpec_X.callback = Sdl_FillAudioBuffer_Callback;
  SdlAudioSpec_X.userdata = &FullAudioBuffer_X;

  /* Open the audio device, forcing the desired format */
  Sts_i = SDL_OpenAudio(&SdlAudioSpec_X, nullptr);
  SDL_CHK_IF_ERR(Sts_i, "Unable to SDL_OpenAudio", Sts_E);
  EXPECT_EQ(Sts_E, BOF_ERR_NO_ERROR);

  /* Load the audio data ... */
//  EXPECT_EQ(BOF::Bof_ReadFile("./data/FfmpegTool/AudioOut.PCM", DiskAudioBuffer_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::Bof_ReadFile("./data/FfmpegTool/AudioOut_001.PCM", DiskAudioBuffer_X), BOF_ERR_NO_ERROR);
  FullAudioBuffer_X.SetStorage(DiskAudioBuffer_X.Size_U64 * 4 / 3, DiskAudioBuffer_X.Size_U64 * 4 / 3, nullptr);
  EXPECT_EQ(BOF2D::Bof_24sleTo32sle(DiskAudioBuffer_X, FullAudioBuffer_X), BOF_ERR_NO_ERROR);
  printf("Playback %d Byte of 32 bits sample\n", FullAudioBuffer_X.Size_U64);
  /* Let the callback function play the audio chunk */
  SDL_PauseAudio(0);

  /* Do some processing */

  ;;;;;

  /* Wait for sound to complete */
  while (FullAudioBuffer_X.RemainToRead())
  {
    printf("Monitor remaining %zd...\n", FullAudioBuffer_X.RemainToRead());
    SDL_Delay(100);
  }
  SDL_CloseAudio();
}

#endif

TEST(Bof2d_ffmpeg_Test, FfmpegTool)
{
  BOF2D::BOF2D_VID_DEC_OUT VidDecOut_X;
  BOF2D::BOF2D_AUD_DEC_OUT AudDecOut_X;
  uint64_t VideoFrameCounter_U64, AudioFrameCounter_U64;
  int16_t *pAudioData_S16;
  uint32_t i_U32 , *pVideoData_U32;
  bool EncodeData_B;
  BOF2D::Bof2dAvCodec AvCodec;
  BOF::BOF_BUFFER DiskAudioBuffer_X, FullAudioBuffer_X, Ok_X, ConvAudioBuffer_X;

  //goto l;

  //First decoder before encoder !!!
#if 0
  EXPECT_EQ(AvCodec.OpenDecoder("./data/Sample24bit96kHz5.1.wav", "", "--A_NBCHNL=6;--A_LAYOUT=0x3F;--A_RATE=48000;--A_BPS=16;--A_DEMUX"), BOF_ERR_NO_ERROR);
  //  EXPECT_EQ(AvCodec.OpenDecoder("./data/Sample-15s.mp3", "", "--A_NBCHNL=2;--A_LAYOUT=0x03;--A_RATE=48000;--A_BPS=32;--A_DEMUX"), BOF_ERR_NO_ERROR);
#else
//  EXPECT_EQ(AvCodec.OpenDecoder("./data/sample-mp4-file.mp4", "--V_WIDTH=160;--V_HEIGHT=120;--V_BPS=32;--V_THREAD=1",
//    "--A_NBCHNL=2;--A_LAYOUT=3;--A_RATE=48000;--A_BPS=24;--A_DEMUX;--A_THREAD=1"), BOF_ERR_NO_ERROR);
  EXPECT_EQ(AvCodec.OpenDecoder("./data/sample-mp4-file.mp4",
    //"--V_WIDTH=160;--V_HEIGHT=120;--V_BPS=32;--V_THREAD=1",
    "",
    //"--A_DEMUX");
    "--A_NBCHNL=2;--A_LAYOUT=3;--A_RATE=48000;--A_BPS=24;--A_DEMUX"), BOF_ERR_NO_ERROR);
#endif
  EXPECT_EQ(AvCodec.OpenEncoder(BOF2D::BOF2D_AV_CONTAINER_FORMAT::BOF2D_AV_CONTAINER_FORMAT_NONE,
    //"--V_BASEFN=./data/FfmpegTool/VideoOut;--V_FMT=PNG;--V_QUALITY=9",
    "--V_BASEFN=./data/FfmpegTool/VideoOut",
    //"--A_BASEFN=./data/FfmpegTool/AudioOut;--A_NBCHNL=2;--A_FMT=PCM;--A_CHUNK");
    "--A_BASEFN=./data/FfmpegTool/AudioOut;--A_NBCHNL=1;--A_CHUNKSIZE= 801, 801, 801, 801, 800"), BOF_ERR_NO_ERROR);
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
//        printf("%06zd: Got Video %zx/%zx:%p Ls %d %dx%d\n", VideoFrameCounter_U64, VidDecOut_X.Data_X.Size_U64, VidDecOut_X.Data_X.Capacity_U64, VidDecOut_X.Data_X.pData_U8,
//          VidDecOut_X.LineSize_S32, VidDecOut_X.Size_X.Width_U32, VidDecOut_X.Size_X.Height_U32);
//        printf("        Got Data %08x %08x %08x %08x %08x %08x %08x %08x\n", pVideoData_U32[0], pVideoData_U32[1], pVideoData_U32[2], pVideoData_U32[3], pVideoData_U32[4], pVideoData_U32[5], pVideoData_U32[6], pVideoData_U32[7]);
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
//        printf("%06zd: Got Audio %zx/%zx:%p nbs %d ch %d layout %zx Rate %d\n", AudioFrameCounter_U64, AudDecOut_X.InterleavedData_X.Size_U64, AudDecOut_X.InterleavedData_X.Capacity_U64, AudDecOut_X.InterleavedData_X.pData_U8,
//          AudDecOut_X.NbSample_U32, AudDecOut_X.NbChannel_U32, AudDecOut_X.ChannelLayout_U64, AudDecOut_X.SampleRateInHz_U32);
//        printf("        Got Data %04x %04x %04x %04x %04x %04x %04x %04x\n", pAudioData_S16[0], pAudioData_S16[1], pAudioData_S16[2], pAudioData_S16[3], pAudioData_S16[4], pAudioData_S16[5], pAudioData_S16[6], pAudioData_S16[7]);
      }
    }
    if (EncodeData_B)
    {
      EXPECT_EQ(AvCodec.BeginWrite(VidDecOut_X, AudDecOut_X), BOF_ERR_NO_ERROR);
      EXPECT_EQ(AvCodec.EndWrite(), BOF_ERR_NO_ERROR);
    }
    EXPECT_EQ(AvCodec.EndRead(), BOF_ERR_NO_ERROR);

    //if ((VideoFrameCounter_U64 > 512))	// || (AudioFrameCounter_U64 > 512))
    if ((VideoFrameCounter_U64) && (VideoFrameCounter_U64 > 250))
    {
      break;
    }
    else if ((AudioFrameCounter_U64) && (AudioFrameCounter_U64 > 512))
    {
      break;
    }
  }
  EXPECT_EQ(AvCodec.CloseEncoder(), BOF_ERR_NO_ERROR);
  EXPECT_EQ(AvCodec.CloseDecoder(), BOF_ERR_NO_ERROR);

  l:
  //transcode 24-32
  EXPECT_EQ(BOF::Bof_ReadFile("./data/FfmpegTool/AudioOut_001.PCM", DiskAudioBuffer_X), BOF_ERR_NO_ERROR);
  FullAudioBuffer_X.SetStorage(DiskAudioBuffer_X.Size_U64 * 4 / 3, DiskAudioBuffer_X.Size_U64 * 4 / 3, nullptr);
  EXPECT_EQ(BOF2D::Bof_24sleTo32sle(DiskAudioBuffer_X, FullAudioBuffer_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF::Bof_WriteFile(BOF::BOF_FILE_PERMISSION_ALL_FOR_ALL, "./data/FfmpegTool/A32_AudioOut_001.PCM", FullAudioBuffer_X), BOF_ERR_NO_ERROR);
  //Compare to data file generated with a --A_BPS=32 option
#if 0
  EXPECT_EQ(BOF::Bof_ReadFile("./data/AudioOut_001.PCM", Ok_X), BOF_ERR_NO_ERROR);
  ConvAudioBuffer_X.SetStorage(0x100000, 0x100000, nullptr);
  EXPECT_EQ(BOF2D::Bof_32sleTo24sle(FullAudioBuffer_X, ConvAudioBuffer_X), BOF_ERR_NO_ERROR);

  BOF::BOF_DUMP_MEMORY_ZONE_PARAM DumpMemoryZoneParam_X;
  std::string Dump_S;

  DumpMemoryZoneParam_X.AccessSize_E = BOF::BOF_ACCESS_SIZE::BOF_ACCESS_SIZE_8;
  DumpMemoryZoneParam_X.NbItemToDump_U32 = 128;
  DumpMemoryZoneParam_X.NbItemPerLine_U32 = 32;
  for (i_U32 = 0; i_U32 < Ok_X.Size_U64; i_U32++)
  {
    if (i_U32 % 4)
    {
      if (Ok_X.pData_U8[i_U32] != FullAudioBuffer_X.pData_U8[i_U32])
      {
        printf("Error at offset %d-> dump memory 33 bytes before\n", i_U32);
        DumpMemoryZoneParam_X.pMemoryZone = &Ok_X.pData_U8[i_U32 - 33];
        Dump_S = BOF::Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
        printf("Ref:\n%s\n", Dump_S.c_str());

        DumpMemoryZoneParam_X.pMemoryZone = &FullAudioBuffer_X.pData_U8[i_U32 - 33];
        Dump_S = BOF::Bof_DumpMemoryZone(DumpMemoryZoneParam_X);
        printf("\nBad:\n%s\n", Dump_S.c_str());

        printf("jj");
      }
    }
  }
#endif
}
