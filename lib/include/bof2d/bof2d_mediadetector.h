/*
 * Copyright (c) 2020-2030, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bof module interface to identify the media characteristics
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
#include <bofstd/bofsystem.h>

#include <bof2d/bof2d.h>

#if defined(BOF2D_BUILD_MEDIAINFO_AS_LIBRARY)
#include "MediaInfo/MediaInfo.h" //Staticly-loaded library (.lib or .a or .so)
#define MediaInfoNameSpace MediaInfoLib;
 //#pragma message("MediaInfoNameSpace MediaInfoLib")
#else 
#if defined(_WIN32)
#include <winsock2.h>	//Needed because MediaInfoDLL.h include windows.h
#endif
#include "MediaInfoDLL/MediaInfoDLL.h" //Dynamicly-loaded library (.dll or .so)
#define MediaInfoNameSpace MediaInfoDLL;
 //#pragma message("MediaInfoNameSpace MediaInfoDLL")
#endif
using namespace MediaInfoNameSpace;

BEGIN_BOF2D_NAMESPACE()

class BOF2D_EXPORT BofMediaDetector
{
public:
  enum class ResultFormat
  {
    Text, Html, Json
  };
  enum class MediaStreamType
  {
    General, Video, Audio, Text, Other, Image, Menu
  };
  enum class InfoType
  {
    Name, Text, Measure, Options, Name_Text, Measure_Text, Info, HowTo, Domain
  };
  BofMediaDetector();
  virtual ~BofMediaDetector();

  std::string Option(const std::string &_rOption_S);
  BOFERR ParseFile(const BOF::BofPath &_rPathName, ResultFormat _ResultFormat_E, std::string &_rResult_S);
  BOFERR ParseBuffer(const BOF::BOF_BUFFER &_rBuffer_X, ResultFormat _ResultFormat_E, std::string &_rResult_S, uint64_t &_rOffsetInBuffer_U64);
  BOFERR Query(MediaStreamType _MediaStreamType_E, const std::string &_rParam_S, InfoType _InfoType_E, std::string &_rResult_S);

private:
  MediaInfo mMediaInfo;
};
END_BOF2D_NAMESPACE()