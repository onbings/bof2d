include(CMakeFindDependencyMacro)

macro(find_package_dependency)
  # When loading the *Config.cmake we should
  # call find_dependency which is just a wrapper
  # around find_package to display better
  # messages to the user. When directly dealing
  # with our CMakeLists.txt, we should call
  # find_package directly
  if(FROM_CONFIG_FILE)
     find_dependency(${ARGN})
  else()
     find_package(${ARGN})
  endif()

endmacro()

# ===========================
# == OPTIONAL DEPENDENCIES ==
# ===========================

# ===========================
# == OPTIONAL DEPENDENCIES ==
# ===========================


if(BOF2D_BUILD_TESTS)
  find_package(GTest REQUIRED)
endif()

# ===========================
# == REQUIRED DEPENDENCIES ==
# ===========================

#set(CMAKE_FIND_DEBUG_MODE TRUE)
find_package(bofstd REQUIRED)
#set(CMAKE_FIND_DEBUG_MODE FALSE)
if(MSVC)
message("Building for Windows")
find_package(glfw3 REQUIRED)
else()
message("Building for Linux")
endif()
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)	#Freetype::Freetype

find_package(libyuv REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ZenLib REQUIRED)
find_package(MediaInfoLib REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(SDL2 REQUIRED)

#message("===SDL2_FOUND===========>" ${SDL2_FOUND})
#message("===SDL2_INCLUDE_DIRS===========>" ${SDL2_INCLUDE_DIRS})
#message("===SDL2_LIBRARIES===========>" ${SDL2_LIBRARIES})
message("===FREETYPE_FOUND===========>" ${FREETYPE_FOUND})
message("===FREETYPE_INCLUDE_DIRS===========>" ${FREETYPE_INCLUDE_DIRS})
message("===FREETYPE_INCLUDE_DIR_ft2build===========>" ${FREETYPE_INCLUDE_DIR_ft2build})
message("===FREETYPE_INCLUDE_DIR_freetype2===========>" ${FREETYPE_INCLUDE_DIR_freetype2})
message("===FREETYPE_LIBRARIES===========>" ${FREETYPE_LIBRARIES})
message("===FREETYPE_VERSION_STRING===========>" ${FREETYPE_VERSION_STRING})
 
find_path(STB_INCLUDE_DIRS "stb_image.h")
message("===STB_INCLUDE_DIRS===========>" ${STB_INCLUDE_DIRS})

##target_include_directories(main PRIVATE ${STB_INCLUDE_DIRS})

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h )
find_library(AVCODEC_LIBRARY avcodec)
message("===AVCODEC_INCLUDE_DIR===========>" ${AVCODEC_INCLUDE_DIR})
message("===AVCODEC_LIBRARY===========>" ${AVCODEC_LIBRARY})

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h )
find_library(AVDEVICE_LIBRARY avdevice)
message("===AVDEVICE_INCLUDE_DIR===========>" ${AVDEVICE_INCLUDE_DIR})
message("===AVDEVICE_LIBRARY===========>" ${AVDEVICE_LIBRARY})

find_path(AVFILTER_INCLUDE_DIR libavfilter/avfilter.h )
find_library(AVFILTER_LIBRARY avfilter)
message("===AVFILTER_INCLUDE_DIR===========>" ${AVFILTER_INCLUDE_DIR})
message("===AVFILTER_LIBRARY===========>" ${AVFILTER_LIBRARY})

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h )
find_library(AVFORMAT_LIBRARY avformat)
message("===AVFORMAT_INCLUDE_DIR===========>" ${AVFORMAT_INCLUDE_DIR})
message("===AVFORMAT_LIBRARY===========>" ${AVFORMAT_LIBRARY})

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h )
find_library(AVUTIL_LIBRARY avutil)
message("===AVUTIL_INCLUDE_DIR===========>" ${AVUTIL_INCLUDE_DIR})
message("===AVUTIL_LIBRARY===========>" ${AVUTIL_LIBRARY})

find_path(SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h )
find_library(SWRESAMPLE_LIBRARY swresample)
message("===SWRESAMPLE_INCLUDE_DIR===========>" ${SWRESAMPLE_INCLUDE_DIR})
message("===SWRESAMPLE_LIBRARY===========>" ${SWRESAMPLE_LIBRARY})

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h )
find_library(SWSCALE_LIBRARY swscale)
message("===SWSCALE_INCLUDE_DIR===========>" ${SWSCALE_INCLUDE_DIR})
message("===SWSCALE_LIBRARY===========>" ${SWSCALE_LIBRARY})

find_path(POSTPROC_INCLUDE_DIR libpostproc/postprocess.h )
find_library(POSTPROC_LIBRARY postproc)
message("===POSTPROC_INCLUDE_DIR===========>" ${POSTPROC_INCLUDE_DIR})
message("===POSTPROC_LIBRARY===========>" ${POSTPROC_LIBRARY})



