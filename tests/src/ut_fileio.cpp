/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bof2d/fileio class
 *
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include "gtestrunner.h"
#include <bofstd/boffs.h>
#include <bof2d/bof2d_fileio.h>
 //USE_BOF2D_NAMESPACE()

TEST(Bof2d_FileIo_Test, ReadWrite)
{
  BOF2D::BOF_VIDEO_FRAME_DATA FrameData_X;
  BOF::BOF_SIZE WndSize_X(640, 480);

  FrameData_X.Reset();
  EXPECT_NE(BOF2D::Bof_ReadGraphicFile("./data/marble.dontexist", 0, FrameData_X), BOF_ERR_NO_ERROR);
  FrameData_X.Reset();
  EXPECT_EQ(BOF2D::Bof_ReadGraphicFile("./data/marble.bmp", 0, FrameData_X), BOF_ERR_NO_ERROR);

  EXPECT_EQ(BOF2D::Bof_WriteGraphicFile(BOF2D::BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_BMP, "./data/marble.tbd.bmp", 90, FrameData_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF2D::Bof_WriteGraphicFile(BOF2D::BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_PNG, "./data/marble.tbd.png", 90, FrameData_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF2D::Bof_WriteGraphicFile(BOF2D::BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_TGA, "./data/marble.tbd.tga", 90, FrameData_X), BOF_ERR_NO_ERROR);
  EXPECT_EQ(BOF2D::Bof_WriteGraphicFile(BOF2D::BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_JPG, "./data/marble.tbd.jpg", 90, FrameData_X), BOF_ERR_NO_ERROR);

  EXPECT_NE(BOF2D::Bof_WriteGraphicFile(BOF2D::BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_MAX, "./data/marble.tbd.bmp", 90, FrameData_X), BOF_ERR_NO_ERROR);
  EXPECT_NE(BOF2D::Bof_WriteGraphicFile(BOF2D::BOF2D_AV_VIDEO_FORMAT::BOF2D_AV_VIDEO_FORMAT_BMP, "./data/marble2.tbd.bmp", 110, FrameData_X), BOF_ERR_NO_ERROR);

  EXPECT_EQ(BOF2D::Bof_ViewGraphicFile("Image Viewer", WndSize_X, FrameData_X), BOF_ERR_NO_ERROR);
}