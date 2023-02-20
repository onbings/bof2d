include(GoogleTest)

# =============
# == SOURCES ==
# =============
  
set(AUDIO_FILES
	./res/audio/bleep.mp3
	./res/audio/bleep.wav
	./res/audio/breakout.mp3
	./res/audio/powerup.wav
	./res/audio/sample-12s.mp3
	./res/audio/solid.wav
)

set(FONT_FILES
	./res/font/Antonio-Bold.ttf
	./res/font/Antonio-Light.ttf
	./res/font/Antonio-Regular.ttf
	./res/font/arial.ttf
	./res/font/OCRAEXT.TTF
)

set(SHADER_FILES
	./res/shader/particle.fs
	./res/shader/particle.vs
	./res/shader/sprite.fs
	./res/shader/sprite.vs
	./res/shader/text.fs
	./res/shader/text.vs
	./res/shader/text_2d.fs
	./res/shader/text_2d.vs
)

set(TEXTURE_FILES
	./res/texture/awesomeface.png
	./res/texture/particle.png
	./res/texture/particle_face.png
	./res/texture/particle_red.png
)

set(DATA_FILES
  ./data/xmlparser.xml
  ./data/xmlvectorparser.xml
  ./data/jsonparser.json
  ./data/colorbar_1920x1080p59_8bit_captured_by_sdk.422
  ./data/colorbar.jpg
  ./data/colorbar.png
  ./data/football_seal.tga
  ./data/colorbar_in_raw_32b_194x259.raw
  ./data/colorbar_jpg_with_bad_ext.png
  ./data/colorbar_jpg_with_no_ext
  ./data/colorbar_png_with_bad_ext.jpg
  ./data/colorbar_png_with_no_ext
  ./data/colorbar_uncompress_32b.tga
  ./data/colorbar_rle_32b.tga
  ./data/sample-mp4-file.mp4
  ./data/HDRI_Sample_Scene_Balls_HDR.jpg
  ./data/marble.bmp
)

set(2D_FILES
	./src/ut_gameengine.cpp
	./src/ut_2d.cpp
	./src/ut_mediadetector.cpp
	./src/ut_fileio.cpp
	./src/ut_convert.cpp
)
set(API_FILES
	include/gtestrunner.h
	src/main.cpp
)

set(SOURCES_FILES
	${2D_FILES}
	${API_FILES}
)

# For Visual Studio
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/bof2d PREFIX "public" FILES ${PUBLIC_INCLUDES_FILES})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src            PREFIX "src"    FILES ${SOURCES_FILES})

source_group("2d"		FILES ${2D_FILES})
source_group("Api"		FILES ${API_FILES})

# ===========
# == BUILD ==
# ===========

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/bigobj)
else()
    # lots of warnings and all warnings as errors
#    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# Create target
add_executable(bof2d-tests
  ${SOURCES_FILES}
)

# Includes
target_include_directories(bof2d-tests
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/src>
)

#print all cmake var
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames) 
#foreach (_variableName ${_variableNames})
#    message(STATUS "EXISTING VAR==============${_variableName}=${${_variableName}}")
#endforeach() 

message("--BOF2D_VLD-->" ${BOF2D_VLD})
if (${BOF2D_VLD})
set(VLD_ROOT_DIR ${bofstd_DIR}/../../include/bofstd/vld)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
set(VLD_DLL_DIR ${VLD_ROOT_DIR}/bin/Win64)
message("--BOF2D_VLD-->64 bits " ${VLD_DLL_DIR})
elseif()
set(VLD_DLL_DIR ${VLD_ROOT_DIR}/bin/Win)
message("--BOF2D_VLD-->32 bits "  ${VLD_DLL_DIR})
endif()

message("--VLD_ROOT_DIR-->" ${VLD_ROOT_DIR})
message("--VLD_DLL_DIR--->" ${VLD_DLL_DIR})
endif()

# Link
target_link_libraries(bof2d-tests
  PRIVATE
    ONBINGS::bof2d-objects
    GTest::GTest
	ONBINGS::bofstd
	${AVCODEC_LIBRARY}
	${AVDEVICE_LIBRARY}
	${AVFILTER_LIBRARY}
	${AVFORMAT_LIBRARY}
	${AVUTIL_LIBRARY}
	${SWRESAMPLE_LIBRARY}
	${SWSCALE_LIBRARY}
)
if (MSVC)
target_link_libraries(bof2d-tests
  PRIVATE
    ONBINGS::bof2d-objects
	Mfplat.lib
	Secur32.lib
	Mfuuid.lib
	Strmiids.lib
)

#Visual Leak Detector: vld
if (${BOF2D_VLD})
target_link_libraries(bof2d-tests
  PRIVATE
    ONBINGS::bof2d-objects
	${VLD_ROOT_DIR}/lib/Win64/vld.lib
)
endif()
endif()

# Object library is not a shared library
# void import directive only for linking
target_compile_definitions(bof2d-tests
  PRIVATE
    -DBOF2D_STATIC_DEFINE=1
)



# Add test and allow them to run in //
gtest_discover_tests(bof2d-tests
  XML_OUTPUT_DIR     ${CMAKE_BINARY_DIR}/GTestReports/
  TEST_PREFIX        bof2d_
   DISCOVERY_TIMEOUT 120
   DISCOVERY_MODE    PRE_TEST
   WORKING_DIRECTORY $<TARGET_FILE_DIR:bof2d>
)

# If you plan on running your unit tests with Valgrind
# it might happen that you need to add 'suppressions'
# to silence known issues. valgrind accepts suppressions
# by taking a file (--log-file=...). The variable
# MEMORYCHECK_SUPPRESSIONS_FILE is not a CMake variable
# but can be used as a convenience to accumulate suppressions
# from all the unit tests. Consequently, if you configure
# valgrind to use that file and define this variable, you will
# have the expected result.
if(MEMORYCHECK_SUPPRESSIONS_FILE)
  file(READ   valgrind_suppression.txt           CONTENT)
  file(APPEND ${MEMORYCHECK_SUPPRESSIONS_FILE} ${CONTENT})
endif()


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/res/audio)
foreach(Filename ${AUDIO_FILES})
  message("Copy " ${Filename} " to " ${CMAKE_CURRENT_BINARY_DIR} "/res/audio")
  file(COPY ${Filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res/audio)
endforeach()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/res/font)
foreach(Filename ${FONT_FILES})
  message("Copy " ${Filename} " to " ${CMAKE_CURRENT_BINARY_DIR} "/res/font")
  file(COPY ${Filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res/font)
endforeach()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/res/shader)
foreach(Filename ${SHADER_FILES})
  message("Copy " ${Filename} " to " ${CMAKE_CURRENT_BINARY_DIR} "/res/shader")
  file(COPY ${Filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res/shader)
endforeach()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/res/texture)
foreach(Filename ${TEXTURE_FILES})
  message("Copy " ${Filename} " to " ${CMAKE_CURRENT_BINARY_DIR} "/res/texture")
  file(COPY ${Filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res/texture)
endforeach()

message("Copy test files from " ${CMAKE_CURRENT_SOURCE_DIR} "/data to " ${CMAKE_CURRENT_BINARY_DIR} "/data")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)
foreach(Filename ${DATA_FILES})
  message("Copy " ${Filename} " to " ${CMAKE_CURRENT_BINARY_DIR} "/data")
  file(COPY ${Filename} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data) 
endforeach()

if (${BOF2D_VLD})
message("Copy vld files from " ${VLD_DLL_DIR}/* " to " ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
#add_custom_command(TARGET bof2d-tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${VLD_DLL_DIR} $<TARGET_FILE_DIR:bof2d-tests>)
endif()