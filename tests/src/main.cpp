#include <bofstd/bofstd.h>
#include <bof2d/bof2d.h>
#include "../include/gtestrunner.h"

int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  int    Rts_i;
  char* p = new char[66]; //Visual Leak Detector

  printf("BofStd version %s\n", BOF::Bof_GetVersion().c_str());
  printf("Bof2d  version %s\n", BOF2D::Bof_GetVersion().c_str());
  //  ::testing::GTEST_FLAG(filter) = "Bof2d_GameEngine_Test.*";
  ::testing::GTEST_FLAG(filter) = "Bof2d_FileIo_Test.*";
  Rts_i = RUN_ALL_TESTS();

#if !defined(NDEBUG)
  std::cout << "\nPress any key followed by enter to to quit ..." << std::endl;
  //	std::cin.ignore();
#endif

  return Rts_i;
}
