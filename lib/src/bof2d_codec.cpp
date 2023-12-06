/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the codec related feature of Bof2d
 *
 * Author:      Bernard HARMEL: onbings@gmail.com
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * History:
 *
 * V 1.00  Nov 13 2022  BHA : Initial release
 */
 //https://stackoverflow.com/questions/72364551/ffmpeg-encoding-mov-with-alpha-from-png-in-c-code

//https://www.bogotobogo.com/FFMpeg/ffmpeg_video_test_patterns_src.php
//ffmpeg - f lavfi - i testsrc = duration = 10 : size = 1280x720 : rate = 30 testsrc.mpg

#include <2d/include/bof2d/bof2d_codec.h>

#include <iostream>

#if 1

#define INBUF_SIZE 4096
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C"
{
#include <libavcodec/avcodec.h>
}
/*
avcl	A pointer to an arbitrary struct of which the first field is a pointer to an AVClass struct.
level	The importance level of the message expressed using a Logging Constant.
fmt	The format string (printf-compatible) that specifies how subsequent arguments are converted to output.
vl	The arguments referenced by the format string.
  */
void avlog_cb(void *avcl, int level, const char *fmt, va_list _va_list)
{
  char buffer[4096];

  va_list params;
  va_copy(params, _va_list);
  auto size = vsnprintf(&buffer[0], sizeof(buffer), fmt, params);
  va_end(params);

  printf("%s", buffer);
}


BEGIN_BOF2D_NAMESPACE()

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize, char *filename)
{
  FILE *f;
  int i;

  f = fopen(filename, "wb");
  fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
  for (i = 0; i < ysize; i++)
  {
    fwrite(buf + i * wrap, 1, xsize, f);
  }
  fclose(f);
}

static BOFERR decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, const char *filename)
{
  BOFERR Rts_E;
  char buf[1024];
  int ret;

  Rts_E = BOF_ERR_ADDRESS;
  ret = avcodec_send_packet(dec_ctx, pkt);
  if (ret >= 0)
  {
    while (ret >= 0)
    {
      Rts_E = BOF_ERR_NO_ERROR;
      ret = avcodec_receive_frame(dec_ctx, frame);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
      {
        Rts_E = BOF_ERR_NO_MORE;
      }
      else if (ret < 0)
      {
        fprintf(stderr, "Error during decoding\n");
        Rts_E = BOF_ERR_FORMAT;
      }
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        printf("saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);

        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
        pgm_save(frame->data[0], frame->linesize[0], frame->width, frame->height, buf);
      }
    }
  }
  return Rts_E;
}

BOFERR Bof2d_Codec(const char *filename, const char *outfilename)
{
  BOFERR Rts_E = BOF_ERR_ENOMEM;
  int NbByteParsed_i;

  const AVCodec *codec;
  AVCodecParserContext *parser;
  AVCodecContext *c = NULL;
  FILE *f;
  AVFrame *frame;
  uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
  uint8_t *data;
  size_t   data_size;
  int eof;
  AVPacket *pkt;

  av_register_all();
  av_log_set_callback(avlog_cb);


  pkt = av_packet_alloc();
  if (pkt)
  {
    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the MPEG-1 video decoder */
    Rts_E = BOF_ERR_NOT_FOUND;
    codec = avcodec_find_decoder(AV_CODEC_ID_H264); // AV_CODEC_ID_MPEG1VIDEO);
    if (codec)
    {
      Rts_E = BOF_ERR_NOT_FOUND;
      parser = av_parser_init(codec->id);
      if (parser)
      {
        Rts_E = BOF_ERR_ENOMEM;
        c = avcodec_alloc_context3(codec);
        if (c)
        {
          Rts_E = BOF_ERR_NOT_OPENED;
          /* For some codecs, such as msmpeg4 and mpeg4, width and height
             MUST be initialized there because this information is not
             available in the bitstream. */

             /* open it */
          if (avcodec_open2(c, codec, NULL) >= 0)
          {
            Rts_E = BOF_ERR_NOT_OPENED;
            f = fopen(filename, "rb");
            if (f)
            {
              Rts_E = BOF_ERR_ENOMEM;
              frame = av_frame_alloc();
              if (frame)
              {
                do
                {
                  /* read raw data from the input file */
                  data_size = fread(inbuf, 1, INBUF_SIZE, f);
                  if (ferror(f))
                  {
                    Rts_E = BOF_ERR_READ;
                    break;
                  }
                  eof = !data_size;

                  /* use the parser to split the data into frames */
                  data = inbuf;
                  while ((data_size > 0) || (eof))
                  {
                    NbByteParsed_i = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                      data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
                    if (NbByteParsed_i < 0)
                    {
                      Rts_E = BOF_ERR_FORMAT;
                      break;
                    }
                    data += NbByteParsed_i;
                    data_size -= NbByteParsed_i;

                    if (pkt->size)
                    {
                      Rts_E = decode(c, frame, pkt, outfilename);
                    }
                    else if (eof)
                    {
                      Rts_E = BOF_ERR_NO_ERROR;
                      break;
                    }
                    else
                    {
                      Rts_E = BOF_ERR_NO_ERROR;
                    }
                    if (Rts_E != BOF_ERR_NO_ERROR)
                    {
                      break;
                    }
                  }
                  if (Rts_E != BOF_ERR_NO_ERROR)
                  {
                    break;
                  }
                } while (!eof);

                /* flush the decoder */
                decode(c, frame, NULL, outfilename);

                fclose(f);

                av_parser_close(parser);
                avcodec_free_context(&c);
                av_frame_free(&frame);
                av_packet_free(&pkt);
              }
            }
          }
        }
      }
    }
  }
  return Rts_E;
}
END_BOF2D_NAMESPACE()


#else
BEGIN_BOF2D_NAMESPACE()

BOFERR Bof2d_Codec(const char *filename, const char *outfilename)
{
  uint8_t p[10000];
  VideoCapture VideoCapture;
  VideoCapture.Init("a.txt", 320, 240, 50, 8000);
  VideoCapture.AddFrame(p);
  VideoCapture.Finish();
}


#define FINAL_FILE_NAME "record.mov"
#define VIDEO_TMP_FILE "tmp.avi"


FILE *fp_write;
extern "C"
{
  static int write_packet(void *opaque, uint8_t *buf, int buf_size)
  {
    struct buffer_data *bd = (struct buffer_data *)opaque;
    printf("ptr  :%p size:%zu\n", bd->ptr, bd->size);
    memcpy(bd->ptr + bd->size, buf, buf_size);
    bd->size = buf_size + bd->size;
    return 1;
  }
  /*
  avcl	A pointer to an arbitrary struct of which the first field is a pointer to an AVClass struct.
level	The importance level of the message expressed using a Logging Constant.
fmt	The format string (printf-compatible) that specifies how subsequent arguments are converted to output.
vl	The arguments referenced by the format string. 
  */
  void avlog_cb(void *avcl, int level, const char *fmt, va_list _va_list)
  {
    char buffer[4096];

    va_list params;
    va_copy(params, _va_list);
    auto size = vsnprintf(&buffer[0], sizeof(buffer), fmt, params);
    va_end(params);

    printf("%s", buffer);
  }
}

VideoCapture::VideoCapture()
{
  av_register_all();
  av_log_set_callback(avlog_cb);
}


VideoCapture::~VideoCapture()
{
  Free();
}


void VideoCapture::Init(std::string filename, int width, int height, int fpsrate, int bitrate) {

  fps = fpsrate;

  int err;
  uint8_t *outbuffer = nullptr;
  outbuffer = (uint8_t *)av_malloc(32768);
  bd = (struct buffer_data *)malloc(sizeof(struct buffer_data));
  bd->ptr = (uint8_t *)av_malloc(1000000000);
  bd->size = 0;
  avio_out = avio_alloc_context(outbuffer, 32768, 1, bd, nullptr, write_packet, nullptr);
  if (!(oformat = av_guess_format("mov", nullptr, nullptr))) {
    std::cout << "Failed to define output format" << std::endl;
    return;
  }
  oformat->video_codec = AV_CODEC_ID_PNG;
  std::cout << "oformat->video_codec " << oformat->video_codec << std::endl;
  if ((err = avformat_alloc_output_context2(&ofctx, oformat, nullptr, nullptr) < 0)) {
    std::cout << "Failed to allocate output context" << std::endl;
    //Free();
    return;
  }
  std::cout << "oformat->video_codec " << oformat->video_codec << std::endl;
  if (!(codec = avcodec_find_encoder(oformat->video_codec))) {
    std::cout << "Failed to find encoder" << std::endl;
    //Free();
    return;
  }

  if (!(videoStream = avformat_new_stream(ofctx, codec))) {
    std::cout << "Failed to create new stream" << std::endl;
    //Free();
    return;
  }

  if (!(cctx = avcodec_alloc_context3(codec))) {
    std::cout << "Failed to allocate codec context" << std::endl;
    //Free();
    return;
  }

  videoStream->codecpar->codec_id = oformat->video_codec;
  videoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  videoStream->codecpar->width = width;
  videoStream->codecpar->height = height;
  videoStream->codecpar->format = AV_PIX_FMT_RGBA;
  videoStream->codecpar->bit_rate = bitrate * 1000;
  videoStream->time_base = { 1, fps };

  avcodec_parameters_to_context(cctx, videoStream->codecpar);
  cctx->time_base = { 1, fps };
  cctx->max_b_frames = 2;
  cctx->gop_size = 12;
  if (videoStream->codecpar->codec_id == AV_CODEC_ID_PNG) {
    //av_opt_set(cctx, "preset", "ultrafast", 0);
  }
  if (ofctx->oformat->flags & AVFMT_GLOBALHEADER) {
    cctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }
  avcodec_parameters_from_context(videoStream->codecpar, cctx);

  if ((err = avcodec_open2(cctx, codec, nullptr)) < 0) {
    std::cout << "Failed to open codec" << std::endl;
    Free();
    return;
  }

  ofctx->pb = avio_out;

  ofctx->flags = AVFMT_FLAG_CUSTOM_IO;
  if ((err = avformat_write_header(ofctx, nullptr)) < 0) {
    std::cout << "Failed to write header" << std::endl;
    Free();
    return;
  }

  //av_dump_format(ofctx, 0, VIDEO_TMP_FILE, 1);
  std::cout << "init com" << std::endl;
}

void VideoCapture::AddFrame(uint8_t *data) {
  int err;
  if (!videoFrame) {

    videoFrame = av_frame_alloc();
    videoFrame->format = AV_PIX_FMT_RGBA;
    videoFrame->width = cctx->width;
    videoFrame->height = cctx->height;

    if ((err = av_frame_get_buffer(videoFrame, 32)) < 0) {
      std::cout << "Failed to allocate picture" << std::endl;
      return;
    }
  }
  std::cout << "finish" << std::endl;
  if (!swsCtx) {
    swsCtx = sws_getContext(cctx->width, cctx->height, AV_PIX_FMT_RGBA, cctx->width, cctx->height, AV_PIX_FMT_RGBA, SWS_BICUBIC, 0, 0, 0);
  }

  int inLinesize[1] = { 4 * cctx->width };


  sws_scale(swsCtx, (const uint8_t *const *)&data, inLinesize, 0, cctx->height, videoFrame->data, videoFrame->linesize);

  videoFrame->pts = (1.0 / 30.0) * 90000 * (frameCounter++);;

  if ((err = avcodec_send_frame(cctx, videoFrame)) < 0) {
    std::cout << "Failed to send frame" << std::endl;
    return;
  }

  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = nullptr;
  pkt.size = 0;

  if (avcodec_receive_packet(cctx, &pkt) == 0) {
    pkt.flags |= AV_PKT_FLAG_KEY;
    av_interleaved_write_frame(ofctx, &pkt);
    av_packet_unref(&pkt);
  }
}

void VideoCapture::Finish() {

  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = nullptr;
  pkt.size = 0;

  for (;;) {
    avcodec_send_frame(cctx, nullptr);
    if (avcodec_receive_packet(cctx, &pkt) == 0) {
      av_interleaved_write_frame(ofctx, &pkt);
      av_packet_unref(&pkt);
    }
    else {
      break;
    }
  }


  av_write_trailer(ofctx);
  /*
  if (!(oformat->flags & AVFMT_NOFILE)) {
      int err = avio_close(ofctx->pb);
      if (err < 0) {
          std::cout <<"Failed to close file"<< std::endl;
      }
  }
  */
  fp_write = fopen(VIDEO_TMP_FILE, "wb");
  if (!feof(fp_write)) {
    int true_size = fwrite(bd->ptr, 1, bd->size, fp_write);
    std::cout << true_size << std::endl;
  }
  fcloseall();

  //Remux();
  //Free();
}

void VideoCapture::Free() {
  if (videoFrame) {
    //std::cout << "videoFrame " << std::endl;
    av_frame_free(&videoFrame);
  }
  if (cctx) {
    //std::cout << "cctx" << std::endl;
    avcodec_free_context(&cctx);
  }
  if (ofctx) {
    //std::cout << "ofctx" << ofctx << std::endl;
    avformat_free_context(ofctx);
  }
  if (swsCtx) {
    //std::cout << "swsCtx" << std::endl;
    sws_freeContext(swsCtx);
  }
  /*
  if (bd->ptr != (void*)0)
  {
      free(bd->ptr);
  }
  free(bd);*/
}

static int read_packet(void *opaque, uint8_t *buf, int buf_size)
{
  struct buffer_data *bd = (struct buffer_data *)opaque;
  buf_size = FFMIN(buf_size, bd->size);
  if (buf_size == 0) return -1;
  //printf("read ptr:%p size:%zu\n", bd->ptr, bd->size);
  /* copy internal buffer data to buf */
  memcpy(buf, bd->ptr, buf_size);
  bd->ptr += buf_size;
  bd->size -= buf_size;
  return buf_size;
}

void VideoCapture::Remux() {
  AVFormatContext *ifmt_ctx = nullptr, *ofmt_ctx = nullptr;
  int err;

  unsigned char *inbuffer = nullptr;
  inbuffer = (unsigned char *)av_malloc(32768);
  ifmt_ctx = avformat_alloc_context();
  AVIOContext *avio_in = avio_alloc_context(inbuffer, 32768, 0, bd, read_packet, nullptr, nullptr);
  ifmt_ctx->pb = avio_in;

  if (!(oformat = av_guess_format(nullptr, nullptr, "h264"))) {
    std::cout << "Failed to define output format";
    return;
  }

  if ((err = avformat_open_input(&ifmt_ctx, "nullptr", 0, 0)) < 0) {
    std::cout << "Failed to open input file for remuxing" << std::endl;
  }
  if ((err = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
    std::cout << "Failed to retrieve input stream information" << std::endl;
  }
  if ((err = avformat_alloc_output_context2(&ofmt_ctx, oformat, nullptr, nullptr))) {
    std::cout << "Failed to allocate output context" << std::endl;
  }

  AVStream *inVideoStream = ifmt_ctx->streams[0];
  AVStream *outVideoStream = avformat_new_stream(ofmt_ctx, nullptr);
  if (!outVideoStream) {
    std::cout << "Failed to allocate output video stream" << std::endl;
  }
  outVideoStream->time_base = { 1, fps };
  avcodec_parameters_copy(outVideoStream->codecpar, inVideoStream->codecpar);
  outVideoStream->codecpar->codec_tag = 0;

  uint8_t *outbuffer = nullptr;
  outbuffer = (uint8_t *)av_malloc(32768);
  res_video = (struct buffer_data *)malloc(sizeof(struct buffer_data));
  res_video->ptr = (uint8_t *)av_malloc(100000000);
  res_video->size = 0;

  if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
    avio_out = avio_alloc_context(outbuffer, 32768, 1, res_video, nullptr, write_packet, nullptr);
    ofmt_ctx->pb = avio_out;
  }
  AVDictionary *opts = nullptr;
  av_dict_set(&opts, "movflags", "frag_keyframe+empty_moov", 0);
  if ((err = avformat_write_header(ofmt_ctx, &opts)) < 0) {
    std::cout << "Failed to write header to output file" << std::endl;
  }

  AVPacket videoPkt;
  int ts = 0;
  while (true) {
    if ((err = av_read_frame(ifmt_ctx, &videoPkt)) < 0) {
      break;
    }
    videoPkt.stream_index = outVideoStream->index;
    videoPkt.pts = ts;
    videoPkt.dts = ts;
    videoPkt.duration = av_rescale_q(videoPkt.duration, inVideoStream->time_base, outVideoStream->time_base);
    ts += videoPkt.duration;
    videoPkt.pos = -1;
    if ((err = av_interleaved_write_frame(ofmt_ctx, &videoPkt)) < 0) {
      std::cout << "Failed to mux packet" << std::endl;
      av_packet_unref(&videoPkt);
      break;
    }
    av_packet_unref(&videoPkt);
  }

  av_write_trailer(ofmt_ctx);
  std::cout << "res_video->size " << res_video->size << std::endl;
  fp_write = fopen(FINAL_FILE_NAME, "wb");
  if (!feof(fp_write)) {
    int true_size = fwrite(res_video->ptr, 1, res_video->size, fp_write);
    std::cout << true_size << std::endl;
  }
  fcloseall();
}
END_BOF2D_NAMESPACE()

#endif

