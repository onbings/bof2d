/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the codec related feature of Bof2d
 *
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * History:
 *
 * V 1.00  Nov 13 2022  BHA : Initial release
 */
 //https://ffmpeg.org/documentation.html
 //https://ffmpeg.org/doxygen/trunk/index.html
 //https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/decode_video.c
 //https://ffmpeg.org/doxygen/trunk/decoding__encoding_8c-source.html
 //====!!>http://dranger.com/ffmpeg/tutorial01.html
#pragma once
//#include <bof2d/bof2d.h>
#pragma message("!!!!!!!!!!!!! CPP => TODO ADAPT THIS LINE AND REMOVE FILE FORM bofstd !!!!!!!!!!!!!")
#include <2d/include/bof2d/bof2d.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

BEGIN_BOF2D_NAMESPACE()
//BOF2D_EXPORT BOFERR Bof_ReadGraphicFile(intptr_t Io, uint32_t _RecordIndex_U32, bool _TopDown_B, uint8_t _BytePerPixel_UB, uint8_t *_pData_UB, uint32_t _Width_U32, uint32_t _Height_U32, uint32_t *_pRecordSize_U32);
BOF2D_EXPORT BOFERR Bof2d_Codec(const char *filename, const char *outfilename);



#if 1
BOFERR Bof2d_Codec(const char *filename, const char *outfilename);
#else

class VideoCapture 
{
public:

  VideoCapture();
  ~VideoCapture();

  void Init(std::string name, int width, int height, int fpsrate, int bitrate);
  void AddFrame(uint8_t *data);
  void Finish();

private:

  AVOutputFormat *oformat = nullptr;
  AVFormatContext *ofctx = nullptr;
  AVIOContext *avio_out = nullptr;
  AVStream *videoStream = nullptr;
  AVFrame *videoFrame = nullptr;

  AVCodec *codec = nullptr;
  AVCodecContext *cctx = nullptr;
  struct buffer_data *bd = nullptr;
  struct buffer_data *res_video = nullptr;
  SwsContext *swsCtx = nullptr;
  //FILE *fp_write;
  char *filename = nullptr;
  //int buf_len;
  int frameCounter = 0;

  int fps = 0;

  void Free();

  void Remux();
};

struct buffer_data 
{
  uint8_t *ptr;
  size_t size; ///< size left in the buffer
};

#endif

END_BOF2D_NAMESPACE()