#include <bof2d/bof2d.h>
#include "../include/gtestrunner.h"


int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  int    Rts_i;

  ::testing::GTEST_FLAG(filter) = "GameEngin2d_Test.*";
  Rts_i = RUN_ALL_TESTS();

#if !defined(NDEBUG)
  std::cout << "\nPress any key followed by enter to to quit ..." << std::endl;
  //	std::cin.ignore();
#endif

  return Rts_i;
}
