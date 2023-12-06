/*
   Copyright (c) 2000-2026, OnBings. All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   This module defines the interface to the Ffmpeg video lib

   Author:      Bernard HARMEL: onbings@gmail.com
   Web:			    onbings.dscloud.me
   Revision:    1.0

   History:

   V 1.00  Sep 30 2000  BHA : Initial release
 */
#include <bof2d/bof2d_video_decoder.h>

BEGIN_BOF2D_NAMESPACE()

Bof2dVideoDecoder::Bof2dVideoDecoder()
{
  mVidDecState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE;
  mVidDecOptionParam_X.push_back({ nullptr, "V_WIDTH", "Specifies converted picture width","","", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mVidDecOption_X.Width_U32, UINT32, 2, 32768) });
  mVidDecOptionParam_X.push_back({ nullptr, "V_HEIGHT", "Specifies converted picture height","","", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mVidDecOption_X.Height_U32, UINT32, 2, 32768) });
  mVidDecOptionParam_X.push_back({ nullptr, "V_BPS", "Specifies converted picture bit per pixel","","", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mVidDecOption_X.NbBitPerPixel_U32, UINT32, 1, 64) });
  mVidDecOptionParam_X.push_back({ nullptr, "V_THREAD", "Specifies number of thread used by decodeer (affect delay)","","", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mVidDecOption_X.NbThread_U32, UINT32, 1, 64) });
}
Bof2dVideoDecoder::~Bof2dVideoDecoder()
{
  Close();
}

BOFERR Bof2dVideoDecoder::Open(AVFormatContext *_pDecFormatCtx_X, const std::string &_rVidDecOption_S, int &_rAudDecStreamIndex_i)
{
  BOFERR    Rts_E = BOF_ERR_EINVAL;
  int       Sts_i;
  uint32_t  i_U32, BufferImgSize_U32;
  uint8_t *pVidDecBuffer_U8;
  BOF::BofCommandLineParser OptionParser;
  const AVFilter *pBufferSrcFlt_X, *pBufferSinkFlt_X;
  char pFilterParam_c[0x1000];
  AVDictionary *pMetadata_X;
  AVDictionaryEntry *pEntry_X;

#if 0
  int       SrcRange_i, DstRange_i, Brightness_i, Contrast_i, Saturation_i;
  const int *pSwsCoef_i;
  int *pInvCoef_i, *pCoef_i;
#endif

  _rAudDecStreamIndex_i = -1;
  if (_pDecFormatCtx_X)
  {
    Rts_E = BOF_ERR_ALREADY_OPENED;
    if (mVidDecState_E == BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE)
    {
      Close();
      mVidDecOption_X.Reset();
      Rts_E = OptionParser.ToByte(_rVidDecOption_S, mVidDecOptionParam_X, nullptr, nullptr);
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        switch (mVidDecOption_X.NbBitPerPixel_U32)
        {
          case 0:
            mVidDecOption_X.NbBitPerPixel_U32 = 32;
          case 32:
            mPixelFmt_E = AV_PIX_FMT_BGRA;
            mVidDecOut_X.NbChannel_U32 = 4;
            break;

          case 24:
            mPixelFmt_E = AV_PIX_FMT_BGR24;
            mVidDecOut_X.NbChannel_U32 = 3;
            break;

          default:
            Rts_E = BOF_ERR_FORMAT;
            break;
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          for (i_U32 = 0; i_U32 < _pDecFormatCtx_X->nb_streams; i_U32++)
          {
            if (_pDecFormatCtx_X->streams[i_U32]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
              mpVidStream_X = _pDecFormatCtx_X->streams[i_U32];
              mpVidDecCodecParam_X = mpVidStream_X->codecpar;
              break;
            }
          }
          if (mpVidDecCodecParam_X == nullptr)
          {
            FFMPEG_CHK_IF_ERR(AVERROR_STREAM_NOT_FOUND, "No video stream", Rts_E);
          }
          else
          {
            mVidDecStreamIndex_i = i_U32;
            _rAudDecStreamIndex_i = mVidDecStreamIndex_i;
            //av_dict_set(&opts, "b", "2.5M", 0);
  // print out videoStream meta data
    // e.g., rotate 270
            pMetadata_X = mpVidStream_X->metadata;
            if (pMetadata_X != nullptr)
            {
              pEntry_X = nullptr;
              while (pEntry_X = av_dict_get(pMetadata_X, "", pEntry_X, AV_DICT_IGNORE_SUFFIX))
              {
                //printf("Video Metadata %s: %s\n", pEntry_X->key, pEntry_X->value);
                mVidMetadataCollection[pEntry_X->key] = pEntry_X->value;
              }
            }

            mpVidDecCodec_X = avcodec_find_decoder(mpVidDecCodecParam_X->codec_id);
            if (mpVidDecCodec_X == nullptr)
            {
              FFMPEG_CHK_IF_ERR(AVERROR_STREAM_NOT_FOUND, "Could not find decoder for codec " + std::to_string(mpVidDecCodecParam_X->codec_id), Rts_E);
            }
            else
            {
              mpVidDecCodecCtx_X = avcodec_alloc_context3(mpVidDecCodec_X);
              if (mpVidDecCodecCtx_X == nullptr)
              {
                FFMPEG_CHK_IF_ERR(-BOF_ERR_ENOMEM, "Could not avcodec_alloc_context3 for codec " + std::to_string(mpVidDecCodecParam_X->codec_id), Rts_E);
              }
              else
              {
                Sts_i = avcodec_parameters_to_context(mpVidDecCodecCtx_X, mpVidDecCodecParam_X);
                FFMPEG_CHK_IF_ERR(Sts_i, "Error in avcodec_parameters_to_context", Rts_E);
                if (Rts_E == BOF_ERR_NO_ERROR)
                {
                  if (mVidDecOption_X.NbThread_U32 == 0)
                  {
                    mVidDecOption_X.NbThread_U32 = 1;
                  }
                  mpVidDecCodecCtx_X->thread_count = mVidDecOption_X.NbThread_U32;

                  Sts_i = avcodec_open2(mpVidDecCodecCtx_X, mpVidDecCodec_X, nullptr);
                  FFMPEG_CHK_IF_ERR(Sts_i, "Could not avcodec_open2", Rts_E);
                  if (Rts_E == BOF_ERR_NO_ERROR)
                  {
                    mpVidDecFrame_X = av_frame_alloc();
                    if (mpVidDecFrame_X == nullptr)
                    {
                      FFMPEG_CHK_IF_ERR(-BOF_ERR_ENOMEM, "Could not av_frame_alloc mpVidDecFrame_X", Rts_E);
                    }
                    else
                    {
                      mpVidDecFrameConverted_X = av_frame_alloc();
                      if (mpVidDecFrameConverted_X == nullptr)
                      {
                        FFMPEG_CHK_IF_ERR(-BOF_ERR_ENOMEM, "Could not av_frame_alloc mpDestFrame_X", Rts_E);
                      }
                      else
                      {
                        mpVidDecFrameFiltered_X = av_frame_alloc();
                        if (mpVidDecFrameFiltered_X == nullptr)
                        {
                          FFMPEG_CHK_IF_ERR(-BOF_ERR_ENOMEM, "Could not av_frame_alloc mpFrameFiltered_X", Rts_E);
                        }
                        else
                        {
                          //Allocate global video decoder buffer
    //                      BufferImgSize_U32 = av_image_get_buffer_size(mPixelFmt_E, mpVidDecCodecCtx_X->width, mpVidDecCodecCtx_X->height, mVidDecAllocAlignment_i);

                          if (mVidDecOption_X.Width_U32 == 0)
                          {
                            mVidDecOption_X.Width_U32 = mpVidDecCodecCtx_X->coded_width;
                          }
                          if (mVidDecOption_X.Height_U32 == 0)
                          {
                            mVidDecOption_X.Height_U32 = mpVidDecCodecCtx_X->coded_height;
                          }
                          mImgSize_U32 = (mVidDecOption_X.NbBitPerPixel_U32 * mVidDecOption_X.Width_U32 * mVidDecOption_X.Height_U32) / 8;
                          BufferImgSize_U32 = av_image_get_buffer_size(mPixelFmt_E, mVidDecOption_X.Width_U32, mVidDecOption_X.Height_U32, mVidDecAllocAlignment_i);
                          BOF_ASSERT(mImgSize_U32 <= BufferImgSize_U32);
                          pVidDecBuffer_U8 = (uint8_t *)av_malloc(BufferImgSize_U32);
                          if (pVidDecBuffer_U8 == nullptr)
                          {
                            FFMPEG_CHK_IF_ERR(-BOF_ERR_ENOMEM, "Could not av_mallocz global video", Rts_E);
                          }
                          else
                          {
                            //Store global audio decoder buffer characteristics
                            mVidDecOut_X.Data_X.SetStorage(BufferImgSize_U32, 0, pVidDecBuffer_U8);
                            mVidDecOut_X.Size_X.Height_U32 = mVidDecOption_X.Height_U32;
                            mVidDecOut_X.Size_X.Width_U32 = mVidDecOption_X.Width_U32;
                            mVidDecOut_X.LineSize_S32 = (mVidDecOption_X.NbBitPerPixel_U32 * mVidDecOption_X.Width_U32) / 8;
                            mVidDecOut_X.PixelFmt_E = mPixelFmt_E;
                            mVidDecOut_X.FrameRate_X = mpVidStream_X->avg_frame_rate;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        mVidDecTimeBase_X = mpVidStream_X->time_base;
        //mVidDecFramePerSecond_lf = av_q2d(mpVidStream_X->r_frame_rate);
        mVidDurationInSec_lf = av_q2d(mVidDecTimeBase_X) * mpVidStream_X->duration;

        mVidDecColorPrimaries_E = mpVidDecCodecCtx_X->color_primaries;
        mVidDecColorRange_E = mpVidDecCodecCtx_X->color_range;
        mVidDecColorSpace_E = mpVidDecCodecCtx_X->colorspace;
        mVidDecColorTrf_E = mpVidDecCodecCtx_X->color_trc;
        mVidDecInPixFmt_E = mpVidDecCodecCtx_X->pix_fmt;
        /*
        if (mOutputCodec_S == "h264")
        {
          mVidDecVideoOutPixFmt_E = mVidDecInPixFmt_E;
        }
        */
        mNbTotalVidDecFrame_U64 = static_cast<int64_t>(_pDecFormatCtx_X->duration / 1.0e6 * mVidDecOut_X.FrameRate_X.num / mVidDecOut_X.FrameRate_X.den);
        //TODO get pixel format from param
        /*
              if (mpVidDecCodecCtx_X->color_primaries == AVColorPrimaries::AVCOL_PRI_BT2020)
              {
                mVidDecRgbPixFmt_E = AVPixelFormat::AV_PIX_FMT_RGB48LE;
              }
              else
              {
                mVidDecRgbPixFmt_E = AVPixelFormat::AV_PIX_FMT_RGB24;
              }
        */
        Sts_i = av_image_fill_arrays(mpVidDecFrameConverted_X->data, mpVidDecFrameConverted_X->linesize, mVidDecOut_X.Data_X.pData_U8, mPixelFmt_E, mVidDecOption_X.Width_U32, mVidDecOption_X.Height_U32, mVidDecAllocAlignment_i);
        //      Sts_i = av_image_alloc(mpVidDecFrameConverted_X->data, mpVidDecFrameConverted_X->linesize, mpVidDecCodecParam_X->width, mpVidDecCodecParam_X->height, mPixelFmt_E, mVidDecAllocAlignment_i);
        FFMPEG_CHK_IF_ERR(Sts_i, "Error in av_image_fill_arrays", Rts_E);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          mpVidDecFrameConverted_X->width = mVidDecOption_X.Width_U32;
          mpVidDecFrameConverted_X->height = mVidDecOption_X.Height_U32;
          mpVidDecFrameConverted_X->format = mPixelFmt_E;

          //is it the same than mpVidDecCodecParam_X->width, mpVidDecCodecParam_X->height,
          //printf("Convert from %s %dx%d to %s %dx%d\n", av_get_pix_fmt_name(mpVidDecCodecCtx_X->pix_fmt), mpVidDecCodecCtx_X->width, mpVidDecCodecCtx_X->height,
          //  av_get_pix_fmt_name(mPixelFmt_E), mVidDecOption_X.Width_U32, mVidDecOption_X.Height_U32);

          mpVidDecSwsCtx_X = sws_getContext(mpVidDecCodecCtx_X->width, mpVidDecCodecCtx_X->height, mpVidDecCodecCtx_X->pix_fmt,
            mVidDecOption_X.Width_U32, mVidDecOption_X.Height_U32, mPixelFmt_E, SWS_BICUBIC, nullptr, nullptr, nullptr);
          if (mpVidDecSwsCtx_X == nullptr)
          {
            FFMPEG_CHK_IF_ERR(-BOF_ERR_ENOMEM, "Could not sws_getContext", Rts_E);
          }
        }
      }

      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        pBufferSrcFlt_X = avfilter_get_by_name("buffer");
        if (pBufferSrcFlt_X == nullptr)
        {
          FFMPEG_CHK_IF_ERR(-BOF_ERR_NOT_FOUND, "Could not avfilter_get_by_name('buffer')", Rts_E);
        }
        else
        {
          pBufferSinkFlt_X = avfilter_get_by_name("buffersink");
          if (pBufferSinkFlt_X == nullptr)
          {
            FFMPEG_CHK_IF_ERR(-BOF_ERR_NOT_FOUND, "Could not avfilter_get_by_name('buffersink')", Rts_E);
          }
          else
          {
            mpVidDecFilterOut_X = avfilter_inout_alloc();
            if (mpVidDecFilterOut_X == nullptr)
            {
              FFMPEG_CHK_IF_ERR(-BOF_ERR_ENOMEM, "Could not avfilter_inout_alloc out", Rts_E);
            }
            else
            {
              mpVidDecFilterIn_X = avfilter_inout_alloc();

              mpVidDecFltGraph_X = avfilter_graph_alloc();
              if (mpVidDecFilterOut_X == nullptr)
              {
                FFMPEG_CHK_IF_ERR(-BOF_ERR_ENOMEM, "Could not avfilter_inout_alloc out", Rts_E);
              }
              else
              {
                /* buffer video source: the decoded frames from the decoder will be inserted here. */
                snprintf(pFilterParam_c, sizeof(pFilterParam_c),
                  "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                  mpVidDecCodecCtx_X->width, mpVidDecCodecCtx_X->height,
                  mpVidDecCodecCtx_X->pix_fmt,
                  mVidDecTimeBase_X.num, mVidDecTimeBase_X.den,
                  mpVidDecCodecCtx_X->sample_aspect_ratio.num,
                  mpVidDecCodecCtx_X->sample_aspect_ratio.den);


                Sts_i = avfilter_graph_create_filter(&mpVidDecFilterSrcCtx_X, pBufferSrcFlt_X, "in", pFilterParam_c, nullptr, mpVidDecFltGraph_X);
                FFMPEG_CHK_IF_ERR(Sts_i, "Could not avfilter_graph_create_filter In", Rts_E);
                if (Rts_E == BOF_ERR_NO_ERROR)
                {
                  /* buffer video sink: to terminate the filter chain. */
                  Sts_i = avfilter_graph_create_filter(&mpVidDecFilterSinkCtx_X, pBufferSinkFlt_X, "out", nullptr, nullptr, mpVidDecFltGraph_X);
                  FFMPEG_CHK_IF_ERR(Sts_i, "Could not avfilter_graph_create_filter Out", Rts_E);
                  if (Rts_E == BOF_ERR_NO_ERROR)
                  {

                    /*
                     * Set the endpoints for the filter graph. The filter_graph will
                     * be linked to the graph described by filters_descr.
                     */

                     /*
                      * The buffer source output must be connected to the input pad of
                      * the first filter described by filters_descr; since the first
                      * filter input label is not specified, it is set to "in" by
                      * default.
                      */
                    mpVidDecFilterOut_X->name = av_strdup("in");
                    mpVidDecFilterOut_X->filter_ctx = mpVidDecFilterSrcCtx_X;
                    mpVidDecFilterOut_X->pad_idx = 0;
                    mpVidDecFilterOut_X->next = nullptr;

                    /*
                     * The buffer sink input must be connected to the output pad of
                     * the last filter described by filters_descr; since the last
                     * filter output label is not specified, it is set to "out" by
                     * default.
                     */
                    mpVidDecFilterIn_X->name = av_strdup("out");
                    mpVidDecFilterIn_X->filter_ctx = mpVidDecFilterSinkCtx_X;
                    mpVidDecFilterIn_X->pad_idx = 0;
                    mpVidDecFilterIn_X->next = nullptr;

                    Sts_i = avfilter_graph_parse_ptr(mpVidDecFltGraph_X, "yadif=mode=1", &mpVidDecFilterIn_X, &mpVidDecFilterOut_X, nullptr);
                    FFMPEG_CHK_IF_ERR(Sts_i, "Could not avfilter_graph_parse_ptr", Rts_E);
                    if (Rts_E == BOF_ERR_NO_ERROR)
                    {
                      Sts_i = avfilter_graph_config(mpVidDecFltGraph_X, nullptr);
                      FFMPEG_CHK_IF_ERR(Sts_i, "Could not avfilter_graph_config", Rts_E);
                      if (Rts_E == BOF_ERR_NO_ERROR)
                      {
                        mVidDecState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_READY;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

      if (Rts_E != BOF_ERR_NO_ERROR)
      {
        Close();
      }
    }
  }
  return Rts_E;
}

BOFERR Bof2dVideoDecoder::Close()
{
  BOFERR Rts_E;
  bool VideoBufferHasBeenFreeed_B = false;

  avfilter_graph_free(&mpVidDecFltGraph_X);
  avfilter_inout_free(&mpVidDecFilterIn_X);
  avfilter_inout_free(&mpVidDecFilterOut_X);

  sws_freeContext(mpVidDecSwsCtx_X);
  mpVidDecSwsCtx_X = nullptr;
  if (mpVidDecFrame_X)
  {
    //If we have used Begin/EndRead method we check that here that the buffer in mpVidDecFrameConverted_X is the same than mpVidDecFrameConverted_X->data[0]. 
    if (mpVidDecFrameConverted_X->data[0] == mVidDecOut_X.Data_X.pData_U8)
    {
      mVidDecOut_X.Data_X.Reset();
      VideoBufferHasBeenFreeed_B = true;  //Done by av_freep(&mpAudDecFrame_X->data[0]); just below
    }
    av_freep(&mpVidDecFrame_X->data[0]);
    av_frame_free(&mpVidDecFrame_X);
  }
  if (!VideoBufferHasBeenFreeed_B)
  {
    //If we just open/close decoder without calling read we need to free mVidDecOut_X.Data_X.pData_U8 here (see remark above)
    av_freep(&mVidDecOut_X.Data_X.pData_U8);
    mVidDecOut_X.Data_X.Reset();
  }
  if (mpVidDecFrameConverted_X)
  {
    av_freep(&mpVidDecFrameConverted_X->data[0]);
    av_frame_free(&mpVidDecFrameConverted_X);
  }
  if (mpVidDecFrameFiltered_X)
  {
    av_freep(&mpVidDecFrameFiltered_X->data[0]);
    av_frame_free(&mpVidDecFrameFiltered_X);
  }
  //avcodec_close: Do not use this function. Use avcodec_free_context() to destroy a codec context(either open or closed)
  avcodec_free_context(&mpVidDecCodecCtx_X);

  av_freep(&mVidDecOut_X.Data_X.pData_U8);

  mVidDecState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE;
  mVidDecOption_X.Reset();
  mVidDecOut_X.Reset();
  mPixelFmt_E = AV_PIX_FMT_NONE;
  mImgSize_U32 = 0;

  //mVidDecPacket_X
  mVidDecStreamIndex_i = -1;
  mpVidDecCodecParam_X = nullptr;
  mpVidDecCodec_X = nullptr;
  mpVidDecFrame_X = nullptr;
  mpVidDecFrameConverted_X = nullptr;
  mpVidDecFrameFiltered_X = nullptr;

  mVidDecColorPrimaries_E = AVColorPrimaries::AVCOL_PRI_RESERVED;
  mVidDecColorRange_E = AVColorRange::AVCOL_RANGE_UNSPECIFIED;
  mVidDecColorTrf_E = AVColorTransferCharacteristic::AVCOL_TRC_RESERVED;
  mVidDecColorSpace_E = AVColorSpace::AVCOL_SPC_RESERVED;

  mVidDecInPixFmt_E = AVPixelFormat::AV_PIX_FMT_NONE;
  mVidDecVideoOutPixFmt_E = AVPixelFormat::AV_PIX_FMT_NONE;
  //  mVidDecRgbPixFmt_E = AVPixelFormat::AV_PIX_FMT_NONE;

  mIsVideoInterlaced_B = false;
  //mVidDecFrameRate_X = { 0, 0 };  //or av_make_q
  mVidDecTimeBase_X = { 0, 0 };
  //mVidDecFramePerSecond_lf = 0.0;

  mNbVidDecPacketSent_U64 = 0;
  mNbVidDecFrameReceived_U64 = 0;
  mNbTotalVidDecFrame_U64 = 0;

  mpVidDecSwsCtx_X = nullptr;

  mpVidDecFilterIn_X = nullptr;
  mpVidDecFilterOut_X = nullptr;
  mpVidDecFltGraph_X = nullptr;
  mpVidDecFilterSinkCtx_X = nullptr;
  mpVidDecFilterSrcCtx_X = nullptr;

  mVidMetadataCollection.clear();
  mpVidStream_X = nullptr;
  mVidDurationInSec_lf = 0.0;

  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}

BOFERR Bof2dVideoDecoder::BeginRead(AVPacket *_pDecPacket_X, BOF2D_VID_DEC_OUT &_rVidDecOut_X)
{
  BOFERR Rts_E = BOF_ERR_EOF;
  int Sts_i;
  uint32_t TotalSizeOfVideoConverted_U32;

  _rVidDecOut_X.Reset();
  if (mVidDecState_E != BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE)
  {
    if (mVidDecState_E == BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_BUSY)
    {
      Rts_E = BOF_ERR_EBUSY;
    }
    else
    {
      mVidDecState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_BUSY;
      mVidDecOut_X.Data_X.Size_U64 = 0;

      Rts_E = BOF_ERR_INDEX;
      if ((_pDecPacket_X) && (_pDecPacket_X->stream_index == mVidDecStreamIndex_i))
      {
        // Convert ffmpeg frame timestamp to real frame number.
        //int64_t numberFrame = (double)((int64_t)pts -  pFormatCtx->streams[videoStreamIndex]->start_time) *  videoBaseTime * videoFramePerSecond;
        Sts_i = avcodec_send_packet(mpVidDecCodecCtx_X, _pDecPacket_X);
        FFMPEG_CHK_IF_ERR(Sts_i, "Cannot avcodec_send_packet Video", Rts_E);
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          mNbVidDecPacketSent_U64++;
          Sts_i = avcodec_receive_frame(mpVidDecCodecCtx_X, mpVidDecFrame_X);
          FFMPEG_CHK_IF_ERR(Sts_i, "Cannot avcodec_receive_frame Video", Rts_E);
          if (Rts_E == BOF_ERR_NO_ERROR)
          {
            mNbVidDecFrameReceived_U64++;
            //printf("VideoFrame Snt/Rcv %zd/%zd Pts %zd Fmt %s Width %d Height %d Buf %x:%p\n", mNbVidDecPacketSent_U64, mNbVidDecFrameReceived_U64, mpVidDecFrame_X->pts, av_get_pix_fmt_name((AVPixelFormat)mpVidDecFrame_X->format),
            //  mpVidDecFrame_X->width, mpVidDecFrame_X->height, mpVidDecFrame_X->linesize[0], mpVidDecFrame_X->data[0]);

            Rts_E = ConvertVideo(TotalSizeOfVideoConverted_U32);
            if (Rts_E == BOF_ERR_NO_ERROR)
            {
              _rVidDecOut_X = mVidDecOut_X;
            }
            av_packet_unref(_pDecPacket_X);
            //break;  //leave while so av_packet_unref(&mVidDecPacket_X);
          }
        }

        if (Rts_E == BOF_ERR_EAGAIN)
        {
          mVidDecState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_PENDING;
          Rts_E = BOF_ERR_NO_ERROR;
        }
      }
    }
  }

  return Rts_E;
}

BOFERR Bof2dVideoDecoder::EndRead()
{
  BOFERR Rts_E = BOF_ERR_EOF;

  if (mVidDecState_E != BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE)
  {
    if (mVidDecState_E == BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_BUSY)
    {
      av_frame_unref(mpVidDecFrame_X);

      mVidDecState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_READY;
      Rts_E = BOF_ERR_NO_ERROR;
    }
    else
    {
      //      Rts_E = (mVidDecState_E == BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_PENDING) ? BOF_ERR_NO_ERROR : BOF_ERR_INVALID_STATE;
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

BOFERR Bof2dVideoDecoder::ConvertVideo(uint32_t &_rTotalSizeOfVideoConverted_U32)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  //uint32_t NbVideoPixel_U32, VideoBufferSize_U32;
  int OutputHeight_i;

  //  if (AudioBufferSize_U32 <= mAudDecOut_X.InterleavedData_X.Capacity_U64)


    //avpicture_fill((AVPicture *)mpVidDecFrame_X, mVidDecOut_X.Data_X.pData_U8, mPixelFmt_E, mpVidDecCodecCtx_X->width, mpVidDecCodecCtx_X->height);
  //  av_image_fill_arrays(mpVidDecFrameConverted_X->data, mpVidDecFrameConverted_X->linesize, mVidDecOut_X.Data_X.pData_U8, mPixelFmt_E, mpVidDecCodecCtx_X->width, mpVidDecCodecCtx_X->height, mVidDecAllocAlignment_i);
  //Quid if upscaling:-> change mVidDecOut_X.Data_X.pData_U8 size to max...
  //one above as constant  av_image_fill_arrays(mpVidDecFrameConverted_X->data, mpVidDecFrameConverted_X->linesize, mVidDecOut_X.Data_X.pData_U8, mPixelFmt_E, mVidDecOption_X.Width_U32, mVidDecOption_X.Width_U32, mVidDecAllocAlignment_i);
  OutputHeight_i = sws_scale(mpVidDecSwsCtx_X, mpVidDecFrame_X->data, mpVidDecFrame_X->linesize, 0,
    mpVidDecFrame_X->height, mpVidDecFrameConverted_X->data, mpVidDecFrameConverted_X->linesize);
  _rTotalSizeOfVideoConverted_U32 = (mVidDecOption_X.NbBitPerPixel_U32 * OutputHeight_i * mVidDecOption_X.Width_U32) / 8; //
  BOF_ASSERT(OutputHeight_i == mVidDecOption_X.Height_U32);
  //  BOF_ASSERT(_rTotalSizeOfVideoConverted_U32 == (mVidDecOption_X.NbBitPerPixel_U32 * mVidDecOption_X.Width_U32 * mVidDecOption_X.Height_U32) / 8);
  BOF_ASSERT(_rTotalSizeOfVideoConverted_U32 == mImgSize_U32);
  BOF_ASSERT(mpVidDecFrameConverted_X->linesize[0] == (mVidDecOption_X.NbBitPerPixel_U32 * mVidDecOption_X.Width_U32) / 8);
  //mVidDecOption_X.Width_U32, mVidDecOption_X.Height_U32
  mpVidDecFrameConverted_X->pkt_pos = mpVidDecFrame_X->pkt_pos;
  mpVidDecFrameConverted_X->pkt_duration = mpVidDecFrame_X->pkt_duration;
  mpVidDecFrameConverted_X->pkt_size = mpVidDecFrame_X->pkt_size;
  mVidDecOut_X.Data_X.Size_U64 = mImgSize_U32;

  mNbTotalVidDecFrame_U64++;
  //  mNbTotaVidDecSample_U64 += _rTotalSizeOfVideoConverted_U32;
  uint32_t *pData_U32 = (uint32_t *)mVidDecOut_X.Data_X.pData_U8;
//  printf("Cnv Video %x->%x:%p Fmt %d Pos %zd Dur %zd Sz %d\n", mpVidDecFrameConverted_X->linesize[0], _rTotalSizeOfVideoConverted_U32, mVidDecOut_X.Data_X.pData_U8,
//    mpVidDecFrameConverted_X->format, mpVidDecFrameConverted_X->pkt_pos, mpVidDecFrameConverted_X->pkt_duration, mpVidDecFrameConverted_X->pkt_size);
//  printf("Cnv Data %08x %08x %08x %08x %08x %08x %08x %08x\n", pData_U32[0], pData_U32[1], pData_U32[2], pData_U32[3], pData_U32[4], pData_U32[5], pData_U32[6], pData_U32[7]);

  //else
  //{
  //  Rts_E = BOF_ERR_EOVERFLOW;
  //}


  return Rts_E;
}

bool Bof2dVideoDecoder::IsVideoStreamPresent()
{
  return(mVidDecStreamIndex_i != -1);
}

AVRational Bof2dVideoDecoder::GetVideoFrameRate()
{
  return mVidDecOut_X.FrameRate_X;
}
END_BOF2D_NAMESPACE()