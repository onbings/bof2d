#include <bofstd/bofstd.h>
#include <bof2d/bof2d.h>
#include "../include/gtestrunner.h"

int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  int    Rts_i;
  BOF2D::BOF2DPARAM Bof2dParam_X;

 // char* p = new char[66]; //Visual Leak Detector

  printf("BofStd version %s\n", BOF::Bof_GetVersion().c_str());
  printf("Bof2d  version %s\n", BOF2D::Bof_GetVersion().c_str());
  EXPECT_EQ(BOF2D::Bof_Initialize(Bof2dParam_X), BOF_ERR_NO_ERROR);
  ::testing::GTEST_FLAG(filter) = "Bof2d_GameEngine_Test.*";  //Change TEST(Bof2d_GameEngine_Test, DISABLED_Loop)
  //::testing::GTEST_FLAG(filter) = "Bof2d_FileIo_Test.*";
  //::testing::GTEST_FLAG(filter) = "Bof2d_Convert_Test.*";
  //::testing::GTEST_FLAG(filter) = "Bof2d_MediaDetector_Test.*";
  //::testing::GTEST_FLAG(filter) = "Bof2d_ffmpeg_Test.*";
  //::testing::GTEST_FLAG(filter) = "Bof2d_ffmpeg_Test.sdl";

  Rts_i = RUN_ALL_TESTS();
  EXPECT_EQ(BOF2D::Bof_Shutdown(), BOF_ERR_NO_ERROR);

#if !defined(NDEBUG)
  std::cout << "\nPress any key followed by enter to to quit ..." << std::endl;
  //	std::cin.ignore();
#endif

  return Rts_i;
}
