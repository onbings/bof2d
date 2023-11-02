/*
   Copyright (c) 2000-2026, OnBings. All rights reserved.

   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
   PURPOSE.

   This module defines the interface to the Ffmpeg audio lib

   Author:      Bernard HARMEL: onbings@gmail.com
   Web:			    onbings.dscloud.me
   Revision:    1.0

   History:

   V 1.00  Sep 30 2000  BHA : Initial release
 */
#include <bofstd/bofstring.h>
//#include <bofstd/bofstringformatter.h>
#include <bofstd/boffs.h>

#include <bof2d/bof2d_audio_encoder.h>

BEGIN_BOF2D_NAMESPACE()

#pragma pack(1)
//https://docs.fileformat.com/audio/wav/#:~:text=up%20to%20date.-,WAV%20File%20Format,contains%20the%20actual%20sample%20data
struct BOF2D_WAV_HEADER
{
  /*000*/  char pRiffHeader_c[4];                 // RIFF Header: "RIFF" Marks the file as a riff file. Characters are each 1 byte long.*/
  /*004*/  uint32_t WavTotalSizeInByteMinus8_U32; // Size of the overall file - 8 bytes, in bytes (32-bit integer). Typically, you�d fill this in after creation. */
  /*008*/  char pWavHeader_c[4];                  //File Type Header. For our purposes, it always equals �WAVE�.
  /*012*/  char pFmtHeader_c[4];                  //Format chunk marker. Includes trailing space and nullptr
  /*016*/  uint32_t FmtChunkSize_U32;             //Length of format data as listed above (Should be 16 for PCM)
  /*020*/  uint16_t AudioFormat_U16;              // Should be 1 for PCM. 3 for IEEE Float 
  /*022*/  uint16_t NbChannel_U16;                //Number of Channels
  /*024*/  uint32_t SampleRateInHz_U32;           //Sample Rate in Hz. Common values are 44100 (CD), 48000 (DAT). Sample Rate = Number of Samples per second, or Hertz.
  /*028*/  uint32_t ByteRate_U32;                 //Number of bytes per second:	(SampleRateInHz_U32 * BitPerSample_U16 * NbChannel_U16) / 8.
  /*032*/  uint16_t SampleAlignment_U16;          //(NbChannel_U16 * BitPerSample_U16) / 8
  /*034*/  uint16_t NbBitPerSample_U16;           //Bits per sample
  /*036*/  char pDataHeader_X[4];                 //�data� chunk header. Marks the beginning of the data section.
  /*040*/  uint32_t DataSizeInByte_U32;           //size of audio: number of samples * num_channels * bit_depth/8
};
#pragma pack()

Bof2dAudioEncoder::Bof2dAudioEncoder()
{
  mAudEncState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE;
  mAudEncOptionParam_X.push_back({ nullptr, "A_BASEFN", "if defined, audio buffer will be saved in this file","","", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mAudEncOption_X.BasePath, PATH, 0, 0) });
  mAudEncOptionParam_X.push_back({ nullptr, "A_CHUNKSIZE", "If specifies, it is followed by 0 (save each chank as it is received) or a circular list of values usedd to slice incoming audio buffer chunk (801,801,801,801,800)", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VECTOR(mAudEncOption_X.SaveChunkSizeInSampleCollection, UINT32, 0, 0) });
  mAudEncOptionParam_X.push_back({ nullptr, "A_NBCHNL", "Specifies the number of audio channel to save","","", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_VARIABLE(mAudEncOption_X.NbChannel_U32, UINT32, 0, 4096) });
  mAudEncOptionParam_X.push_back({ nullptr, "A_FMT", "Specifies the audio format", "", "", BOF::BOFPARAMETER_ARG_FLAG::CMDLINE_LONGOPT_NEED_ARG, BOF_PARAM_DEF_ENUM(mAudEncOption_X.Format_E, BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_PCM, BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_MAX, S_Bof2dAvAudioFormatEnumConverter, BOF2D_AV_AUDIO_FORMAT) });
}

Bof2dAudioEncoder::~Bof2dAudioEncoder()
{
  Close();
}

BOFERR Bof2dAudioEncoder::Open(const std::string &_rOption_S, AVRational &_rVideoFrameRate_X)
{
  BOFERR   Rts_E = BOF_ERR_ECANCELED;
  uint32_t i_U32;
  BOF::BofCommandLineParser OptionParser;
  BOF2D_AUD_ENC_OUT AudEncOut_X;
  BOF2D_AUD_ENC_CHUNK_STATE AudEncChunkState_X;

  if (mAudEncState_E == BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE)
  {
    Close();
    mVideoFrameRate_X = _rVideoFrameRate_X;
    mAudEncOption_X.Reset();
    Rts_E = OptionParser.ToByte(_rOption_S, mAudEncOptionParam_X, nullptr, nullptr);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_INVALID_DST;
      BOF::Bof_CreateDirectory(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER | BOF::BOF_FILE_PERMISSION_READ_FOR_ALL, mAudEncOption_X.BasePath.DirectoryName(true, false));
      if (mAudEncOption_X.BasePath.IsValid())
      {
        BOF::Bof_CleanupDirectory(true, mAudEncOption_X.BasePath.DirectoryName(true, false), false);
        if (mAudEncOption_X.NbChannel_U32 == 0)
        {
          mAudEncOption_X.NbChannel_U32 = 2;
        }
        if (mAudEncOption_X.Format_E == BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_MAX)
        {
          mAudEncOption_X.Format_E = BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_PCM;
        }
        mIoCollection.clear();
        mIoCollection.push_back(AudEncOut_X);  //Entry 0 is for interleaved sample global file

        for (i_U32 = 0; i_U32 < mAudEncOption_X.NbChannel_U32; i_U32++)
        {
          mIoCollection.push_back(AudEncOut_X);    //These one are for per channel audio data
          mAudEncChunkStateCollection.push_back(AudEncChunkState_X);
        }
        Rts_E = CreateFileOut();
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          mAudEncState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_READY;
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

BOFERR Bof2dAudioEncoder::Close()
{
  BOFERR Rts_E;

  CloseFileOut();

  mAudEncState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE;
  mAudEncOption_X.Reset();
  mIoCollection.clear();
  mAudDecOut_X.Reset();

  mNbTotalAudEncFrame_U64 = 0;
  mVideoFrameRate_X = { 0, 0 };

  mAudEncChunkStateCollection.clear();

  Rts_E = BOF_ERR_NO_ERROR;
  return Rts_E;
}


BOFERR Bof2dAudioEncoder::BeginWrite(BOF2D_AUD_DEC_OUT &_rAudDecOut_X)
{
  BOFERR Rts_E = BOF_ERR_ECANCELED;

  if (mAudEncState_E != BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE)
  {
    if (mAudEncState_E == BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_BUSY)
    {
      Rts_E = BOF_ERR_EBUSY;
    }
    else
    {
      mAudEncState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_BUSY;
      mAudDecOut_X = _rAudDecOut_X;
      Rts_E = WriteChunkOut();
      if (Rts_E == BOF_ERR_NO_ERROR)
      {
        mNbTotalAudEncFrame_U64++;
      }
    }
  }
  return Rts_E;
}

BOFERR Bof2dAudioEncoder::EndWrite()
{
  BOFERR Rts_E = BOF_ERR_ECANCELED;

  if (mAudEncState_E != BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE)
  {
    Rts_E = BOF_ERR_EOF;
    if (mAudEncState_E == BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_BUSY)
    {
      mAudEncState_E = BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_READY;
      Rts_E = BOF_ERR_NO_ERROR;
    }
    else
    {
      //      Rts_E = (mAudEncState_E == BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_PENDING) ? BOF_ERR_NO_ERROR : BOF_ERR_INVALID_STATE;
      Rts_E = BOF_ERR_NO_ERROR;
    }
  }
  return Rts_E;
}

BOFERR Bof2dAudioEncoder::CreateFileOut()
{
  BOFERR Rts_E, Sts_E;
  uint32_t i_U32;
  std::string ChannelPath_S, Extension_S = S_Bof2dAvAudioFormatEnumConverter.ToString(mAudEncOption_X.Format_E);  // (mAudEncOption_X.Format_E == BOF2D_AUDIO_FORMAT::BOF2D_AUDIO_FORMAT_WAV) ? ".wav" : ".pcm";

  //Entry 0 is for interleaved sample global file
  Rts_E = BOF::Bof_CreateFile(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER | BOF::BOF_FILE_PERMISSION_READ_FOR_ALL, mAudEncOption_X.BasePath.FullPathNameWithoutExtension(false) + '.' + Extension_S, false, mIoCollection[0].Io);
  if (Rts_E == BOF_ERR_NO_ERROR)
  {
    for (i_U32 = 1; i_U32 < mIoCollection.size(); i_U32++) //Entry 0 is for interleaved sample global file
    {
      ChannelPath_S = BOF::Bof_Sprintf("%s_%03d.%s", mAudEncOption_X.BasePath.FullPathNameWithoutExtension(false).c_str(), i_U32, Extension_S.c_str());
      Sts_E = BOF::Bof_CreateFile(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER | BOF::BOF_FILE_PERMISSION_READ_FOR_ALL, ChannelPath_S, false, mIoCollection[i_U32].Io);
      if (Sts_E != BOF_ERR_NO_ERROR)
      {
        Rts_E = Sts_E;
      }
    }
    Sts_E = WriteHeader();

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = Sts_E;
    }
  }
  return Rts_E;
}

BOFERR Bof2dAudioEncoder::WriteHeader()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  BOF2D_WAV_HEADER WavHeader_X;
  uint32_t i_U32, Nb_U32;
  int64_t Pos_S64;

  if (mAudEncOption_X.Format_E == BOF2D_AV_AUDIO_FORMAT::BOF2D_AV_AUDIO_FORMAT_WAV)
  {
    for (i_U32 = 0; i_U32 < mIoCollection.size(); i_U32++)  //Entry 0 is for interleaved sample global file
    {
      if (mIoCollection[i_U32].Io != BOF::BOF_INVALID_HANDLE_VALUE)
      {
        Pos_S64 = BOF::Bof_GetFileIoPosition(mIoCollection[i_U32].Io);
        if (Pos_S64 != -1)
        {
          BOF::Bof_SetFileIoPosition(mIoCollection[i_U32].Io, 0, BOF::BOF_SEEK_METHOD::BOF_SEEK_BEGIN);
          memcpy(&WavHeader_X.pRiffHeader_c, "RIFF", 4);
          WavHeader_X.WavTotalSizeInByteMinus8_U32 = static_cast<uint32_t>(mIoCollection[i_U32].Size_U64 + sizeof(struct BOF2D_WAV_HEADER) - 8);
          memcpy(&WavHeader_X.pWavHeader_c, "WAVE", 4);
          memcpy(&WavHeader_X.pFmtHeader_c, "fmt ", 4);
          WavHeader_X.FmtChunkSize_U32 = 16;  //PCM
          WavHeader_X.AudioFormat_U16 = 1;    //PCM
          WavHeader_X.NbChannel_U16 = (i_U32 == 0) ? static_cast<uint16_t>(mAudEncOption_X.NbChannel_U32) : 1;
          WavHeader_X.SampleRateInHz_U32 = mAudDecOut_X.SampleRateInHz_U32;
          WavHeader_X.SampleAlignment_U16 = (i_U32 == 0) ? static_cast<uint16_t>((mAudEncOption_X.NbChannel_U32 * mAudDecOut_X.NbBitPerSample_U32) / 8) : static_cast<uint16_t>(mAudDecOut_X.NbBitPerSample_U32 / 8);
          WavHeader_X.NbBitPerSample_U16 = static_cast<uint16_t>(mAudDecOut_X.NbBitPerSample_U32);
          WavHeader_X.ByteRate_U32 = mAudDecOut_X.SampleRateInHz_U32 * WavHeader_X.SampleAlignment_U16;
          memcpy(&WavHeader_X.pDataHeader_X, "data", 4);
          WavHeader_X.DataSizeInByte_U32 = static_cast<uint32_t>(mIoCollection[i_U32].Size_U64);
          Nb_U32 = sizeof(struct BOF2D_WAV_HEADER);
          Rts_E = BOF::Bof_WriteFile(mIoCollection[i_U32].Io, Nb_U32, reinterpret_cast<uint8_t *>(&WavHeader_X));
          //NO    BOF::Bof_SetFileIoPosition(mIoCollection[i_U32], Pos_S64, BOF::BOF_SEEK_METHOD::BOF_SEEK_BEGIN);
        }
        else
        {
          Rts_E = BOF_ERR_INVALID_HANDLE;
        }
      }
    }
  }
  return Rts_E;
}

#define UPDATE_SIZE()         { pAudEncChunkState_X->RemainingChunkSize_U32 -= Nb_U32; \
                                SizeToWrite_U32 -= Nb_U32; \
                                IndexInDataBuffer_U32 += Nb_U32; \
                              }
//printf("    UPD_SZ %d byte written -> DataIndx %d, Still %d to write ChunkSize is %d\n", Nb_U32, IndexInDataBuffer_U32, SizeToWrite_U32, ChunkSize_U32); \

#define UPDATE_SUB_CHUNK()    { pAudEncChunkState_X->SubChunkId_U32++; \
                                pAudEncChunkState_X->SaveChunkSizeCollectionIndex_U32++; \
                                if (pAudEncChunkState_X->SaveChunkSizeCollectionIndex_U32 >= mAudEncOption_X.SaveChunkSizeInSampleCollection.size()) \
                                { \
                                  pAudEncChunkState_X->SaveChunkSizeCollectionIndex_U32 = 0; \
                                } \
                                ChunkSize_U32 = (mAudEncOption_X.SaveChunkSizeInSampleCollection[pAudEncChunkState_X->SaveChunkSizeCollectionIndex_U32] * mAudDecOut_X.NbBitPerSample_U32) / 8; \
                              }
//printf("    NXT_SUB SubChunkId %d Index %d ChunkSize %d\n", pAudEncChunkState_X->SubChunkId_U32, pAudEncChunkState_X->SaveChunkSizeCollectionIndex_U32, ChunkSize_U32); \

BOFERR Bof2dAudioEncoder::WriteChunkOut()
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR, Sts_E;
  uint32_t Nb_U32, ChunkSize_U32, i_U32, SizeToWrite_U32, IndexInDataBuffer_U32;
  uintptr_t OutAudioChunkFile;
  BOF2D_AUD_ENC_CHUNK_STATE *pAudEncChunkState_X;

  //Entry 0 is for interleaved sample global file
  if ((mIoCollection[0].Io != BOF::BOF_INVALID_HANDLE_VALUE)
    && (mAudDecOut_X.InterleavedData_X.pData_U8)
    && (mAudDecOut_X.InterleavedData_X.Size_U64))
  {
    SizeToWrite_U32 = static_cast<uint32_t>(mAudDecOut_X.InterleavedData_X.Size_U64);
    Rts_E = BOF::Bof_WriteFile(mIoCollection[0].Io, SizeToWrite_U32, mAudDecOut_X.InterleavedData_X.pData_U8);
  }
  //if (Rts_E == BOF_ERR_NO_ERROR)
  {
    mIoCollection[0].Size_U64 += SizeToWrite_U32;

    for (i_U32 = 1; i_U32 < mIoCollection.size(); i_U32++) //Entry 0 is for interleaved sample global file
    {
      if ((mIoCollection[i_U32].Io != BOF::BOF_INVALID_HANDLE_VALUE)
        && (mAudDecOut_X.ChannelBufferCollection[i_U32 - 1].pData_U8)
        && (mAudDecOut_X.ChannelBufferCollection[i_U32 - 1].Size_U64)
        )
      {
        SizeToWrite_U32 = static_cast<uint32_t>(mAudDecOut_X.ChannelBufferCollection[i_U32 - 1].Size_U64);
        Sts_E = BOF::Bof_WriteFile(mIoCollection[i_U32].Io, SizeToWrite_U32, mAudDecOut_X.ChannelBufferCollection[i_U32 - 1].pData_U8);
        if (Sts_E == BOF_ERR_NO_ERROR)
        {
          mIoCollection[i_U32].Size_U64 += SizeToWrite_U32;
          if (!mAudEncOption_X.SaveChunkSizeInSampleCollection.empty())
          {
            SizeToWrite_U32 = static_cast<uint32_t>(mAudDecOut_X.ChannelBufferCollection[i_U32 - 1].Size_U64);
            pAudEncChunkState_X = &mAudEncChunkStateCollection[i_U32 - 1];
            if ((mAudEncOption_X.SaveChunkSizeInSampleCollection.size() == 1) && (mAudEncOption_X.SaveChunkSizeInSampleCollection[0] == 0))
            {
              pAudEncChunkState_X->SubChunkId_U32 = 0;
              //Save each chunk in a separate file
              pAudEncChunkState_X->LastChunkPath_S = BOF::Bof_Sprintf("%s_chunk_%08zd_%03d_%03d.pcm", mAudEncOption_X.BasePath.FullPathNameWithoutExtension(false).c_str(), mNbTotalAudEncFrame_U64, i_U32, pAudEncChunkState_X->SubChunkId_U32);
              Sts_E = BOF::Bof_CreateFile(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER | BOF::BOF_FILE_PERMISSION_READ_FOR_ALL, pAudEncChunkState_X->LastChunkPath_S, false, OutAudioChunkFile);
              if (Sts_E == BOF_ERR_NO_ERROR)
              {
                Sts_E = BOF::Bof_WriteFile(OutAudioChunkFile, SizeToWrite_U32, mAudDecOut_X.ChannelBufferCollection[i_U32 - 1].pData_U8);
                BOF::Bof_CloseFile(OutAudioChunkFile);
              }
            }
            else
            {
              ChunkSize_U32 = (mAudEncOption_X.SaveChunkSizeInSampleCollection[pAudEncChunkState_X->SaveChunkSizeCollectionIndex_U32] * mAudDecOut_X.NbBitPerSample_U32) / 8;
              IndexInDataBuffer_U32 = 0;
              //printf("Frm %06zd Chnl %02d Rem %d Sz %d LstFn '%s' ToWrt %d SzIndx %d ChunkSize %d SubChunkId %d\n", mNbTotalAudEncFrame_U64, i_U32, pAudEncChunkState_X->RemainingChunkSize_U32, ChunkSize_U32,
              //  pAudEncChunkState_X->LastChunkPath_S.c_str(), SizeToWrite_U32, pAudEncChunkState_X->SaveChunkSizeCollectionIndex_U32, ChunkSize_U32, pAudEncChunkState_X->SubChunkId_U32);
              if (pAudEncChunkState_X->RemainingChunkSize_U32)
              {
                BOF_ASSERT(pAudEncChunkState_X->LastChunkPath_S != "");
                //Open in append !!
                Sts_E = BOF::Bof_OpenFile(pAudEncChunkState_X->LastChunkPath_S, false, true, OutAudioChunkFile);
                if (Sts_E == BOF_ERR_NO_ERROR)
                {
                  Nb_U32 = (SizeToWrite_U32 <= pAudEncChunkState_X->RemainingChunkSize_U32) ? SizeToWrite_U32 : pAudEncChunkState_X->RemainingChunkSize_U32;
                  Sts_E = BOF::Bof_WriteFile(OutAudioChunkFile, Nb_U32, mAudDecOut_X.ChannelBufferCollection[i_U32 - 1].pData_U8);
                  if (Sts_E == BOF_ERR_NO_ERROR)
                  {
                    //printf("  Append %d bytes to %s Cs %d Rem %d ToWrt %d\n", Nb_U32, pAudEncChunkState_X->LastChunkPath_S.c_str(), ChunkSize_U32, pAudEncChunkState_X->RemainingChunkSize_U32, SizeToWrite_U32);
                    UPDATE_SIZE();
                    if (pAudEncChunkState_X->RemainingChunkSize_U32 == 0)
                    {
                      UPDATE_SUB_CHUNK();
                    }
                  }
                  BOF::Bof_CloseFile(OutAudioChunkFile);
                  if (pAudEncChunkState_X->RemainingChunkSize_U32 == 0)
                  {
                    //printf("    %s final size append is %zd Frm %zd SubChunkId %d\n", pAudEncChunkState_X->LastChunkPath_S.c_str(), BOF::Bof_GetFileSize(pAudEncChunkState_X->LastChunkPath_S), mNbTotalAudEncFrame_U64, pAudEncChunkState_X->SubChunkId_U32);
                  }
                  else
                  {
                    //printf("  %s intermediate size is %zd\n", pAudEncChunkState_X->LastChunkPath_S.c_str(), BOF::Bof_GetFileSize(pAudEncChunkState_X->LastChunkPath_S));
                  }
                }
              }
              SizeToWrite_U32 += pAudEncChunkState_X->RemainingChunkSize_U32;
              //printf("  Rem %d -> ToWrt is %d\n", pAudEncChunkState_X->RemainingChunkSize_U32, SizeToWrite_U32);
              if (SizeToWrite_U32)
              {
                pAudEncChunkState_X->SubChunkId_U32 = 0;
              }
              while ((Sts_E == BOF_ERR_NO_ERROR) && (SizeToWrite_U32))
              {
                pAudEncChunkState_X->RemainingChunkSize_U32 = ChunkSize_U32;

                //Split each chunk in file with a size of Size_U32 bytes
                pAudEncChunkState_X->LastChunkPath_S = BOF::Bof_Sprintf("%s_chunk_%08zd_%03d_%03d.pcm", mAudEncOption_X.BasePath.FullPathNameWithoutExtension(false).c_str(), mNbTotalAudEncFrame_U64, i_U32, pAudEncChunkState_X->SubChunkId_U32);
                Sts_E = BOF::Bof_CreateFile(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER | BOF::BOF_FILE_PERMISSION_READ_FOR_ALL, pAudEncChunkState_X->LastChunkPath_S, false, OutAudioChunkFile);
                if (Sts_E == BOF_ERR_NO_ERROR)
                {
                  Nb_U32 = (SizeToWrite_U32 <= ChunkSize_U32) ? SizeToWrite_U32 : ChunkSize_U32;
                  Sts_E = BOF::Bof_WriteFile(OutAudioChunkFile, Nb_U32, &mAudDecOut_X.ChannelBufferCollection[i_U32 - 1].pData_U8[IndexInDataBuffer_U32]);
                  if (Sts_E == BOF_ERR_NO_ERROR)
                  {
                    //printf("  Create %s and write %d bytes, ChunkSize %d Rem %d ToWrt %d\n", pAudEncChunkState_X->LastChunkPath_S.c_str(), Nb_U32, ChunkSize_U32, pAudEncChunkState_X->RemainingChunkSize_U32, SizeToWrite_U32);
                    UPDATE_SIZE();
                    if (SizeToWrite_U32)
                    {
                      UPDATE_SUB_CHUNK();
                    }
                  }
                  BOF::Bof_CloseFile(OutAudioChunkFile);
                  if (SizeToWrite_U32)
                  {
                    //printf("    %s final size is %zd Frm %zd SubChunkId %d\n", pAudEncChunkState_X->LastChunkPath_S.c_str(), BOF::Bof_GetFileSize(pAudEncChunkState_X->LastChunkPath_S), mNbTotalAudEncFrame_U64, pAudEncChunkState_X->SubChunkId_U32);
                  }
                }
              }
            }
          }
        }
        if (Sts_E != BOF_ERR_NO_ERROR)
        {
          Rts_E = Sts_E;
        }
      }
    }
  }
  return Rts_E;
}

BOFERR Bof2dAudioEncoder::CloseFileOut()
{
  BOFERR Rts_E;
  uint32_t i_U32;

  Rts_E = WriteHeader();
  for (i_U32 = 0; i_U32 < mIoCollection.size(); i_U32++)  //Entry 0 is for interleaved sample global file
  {
    if (mIoCollection[i_U32].Io != BOF::BOF_INVALID_HANDLE_VALUE)
    {
      BOF::Bof_CloseFile(mIoCollection[i_U32].Io);
      mIoCollection[i_U32].Reset();
    }
  }
  return Rts_E;
}

bool Bof2dAudioEncoder::IsAudioStreamPresent()
{
  return mAudEncState_E != BOF2D_AV_CODEC_STATE::BOF2D_AV_CODEC_STATE_IDLE;
}

END_BOF2D_NAMESPACE()