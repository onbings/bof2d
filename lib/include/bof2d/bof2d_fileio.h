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
#include <bofstd/bofpath.h>

 //#include <bof2d/bof2d.h>
#include <bof2d/bof2d_av_codec.h>

BEGIN_BOF2D_NAMESPACE()
struct BOF2D_EXPORT BOF_VIDEO_FRAME_DATA
{
  uint32_t  LineSizeInByte_U32;
  uint32_t  NbChannel_U32;
  BOF::BOF_BUFFER VideoFrameBuffer_X;
  BOF::BOF_SIZE VideoFrameSizeInPixel_X;

  BOF_VIDEO_FRAME_DATA()
  {
    Reset();
  }

  void Reset()
  {
    LineSizeInByte_U32 = 0;
    NbChannel_U32 = 0;
    VideoFrameBuffer_X.Reset();
    VideoFrameSizeInPixel_X.Reset();
  }
};
struct BOF2D_EXPORT BOF_AUDIO_FRAME_DATA
{
  uint32_t SampleRateInHz_U32;
  uint32_t NbBitPerSample_U32;
  uint32_t  NbChannel_U32;
  BOF::BOF_BUFFER AudioFrameBuffer_X;

  BOF_AUDIO_FRAME_DATA()
  {
    Reset();
  }

  void Reset()
  {
    SampleRateInHz_U32 = 0;
    NbBitPerSample_U32 = 0;
    NbChannel_U32 = 0;
    AudioFrameBuffer_X.Reset();
  }
};
BOF2D_EXPORT BOFERR Bof_ReadGraphicFile(const BOF::BofPath &_rPath, int _NbChannelToRead_i, BOF_VIDEO_FRAME_DATA &_rFrameData_X);
BOF2D_EXPORT BOFERR Bof_WriteGraphicFile(BOF2D_AV_VIDEO_FORMAT _Format_E, const BOF::BofPath &_rPath, uint32_t _EncQuality_U32, const BOF_VIDEO_FRAME_DATA &_rFrameData_X);
BOF2D_EXPORT BOFERR Bof_ViewGraphicFile(const std::string &_rTitle_S, BOF::BOF_SIZE &_rWindowSize_X, const BOF_VIDEO_FRAME_DATA &_rVideoFrameData_X);
BOF2D_EXPORT BOFERR Bof_ListenAudioFile(volatile bool *_pStopListening_B, const BOF_AUDIO_FRAME_DATA &_rAudioFrameData_X);

END_BOF2D_NAMESPACE()