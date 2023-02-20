# =============
# == SOURCES ==
# =============

set(PUBLIC_INCLUDES_FILES
)

set(API_FILES
	./include/bof2d/bof2d.h
	./src/bof2d.cpp
	./include/bof2d/bof2d_convert.h
	./src/bof2d_convert.cpp
	./include/bof2d/bof2d_fileio.h
	./src/bof2d_fileio.cpp
	./include/bof2d/bof2d_mediadetector.h
	./src/bof2d_mediadetector.cpp
)

set(CODEC_FILES
	./include/bof2d/bof2d_video_encoder.h
	./src/bof2d_video_encoder.cpp
	./include/bof2d/bof2d_audio_encoder.h
	./src/bof2d_audio_encoder.cpp
	./include/bof2d/bof2d_video_decoder.h
	./src/bof2d_video_decoder.cpp
	./include/bof2d/bof2d_audio_decoder.h
	./src/bof2d_audio_decoder.cpp
	./include/bof2d/bof2d_av_codec.h
	./src/bof2d_av_codec.cpp
)

set(GAME_ENGINE_FILES
	./include/bof2d/engine/bof2d_ball_obj.h
	./src/engine/bof2d_ball_obj.cpp
	./include/bof2d/engine/bof2d_game_engine.h
	./src/engine/bof2d_game_engine.cpp
	./include/bof2d/engine/bof2d_game_obj.h
	./src/engine/bof2d_game_obj.cpp
	./include/bof2d/engine/bof2d_particle_gen.h
	./src/engine/bof2d_particle_gen.cpp
	./src/engine/bof2d_opengl.cpp
	./include/bof2d/engine/bof2d_res_mgr.h
	./src/engine/bof2d_res_mgr.cpp
	./include/bof2d/engine/bof2d_shader.h
	./src/engine/bof2d_shader.cpp
	./include/bof2d/engine/bof2d_sprite_renderer.h
	./src/engine/bof2d_sprite_renderer.cpp
	./include/bof2d/engine/bof2d_text_renderer.h
	./src/engine/bof2d_text_renderer.cpp
	./include/bof2d/engine/bof2d_texture.h
	./src/engine/bof2d_texture.cpp
)

set(SOURCES_FILES
	${API_FILES}
	${CODEC_FILES}
	${GAME_ENGINE_FILES}
)

# For Visual Studio
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/bof2d PREFIX "public" FILES ${PUBLIC_INCLUDES_FILES})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src            PREFIX "src"    FILES ${SOURCES_FILES})

source_group("Api"         FILES ${API_FILES})
source_group("Codec"       FILES ${CODEC_FILES})
source_group("GameEngine"  FILES ${GAME_ENGINE_FILES})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/include/bof2d/bof2d_version_info.h.in ${CMAKE_CURRENT_BINARY_DIR}/bof2d_version_info.h @ONLY)

# ===========
# == BUILD ==
# ===========

if (MSVC)
    # warning level 4 and all warnings as errors
#    add_compile_options(/bigobj)
else()
    # lots of warnings and all warnings as errors
#    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# When creating shared library, we will want to control API visibility. This means, hide symbols that are
# not part of the API. This is great for the end user but not for us as we can no longer test our non API code.
#
# The solution is thus to create an intermediate step which is an object library.
# Object library compiles the files but does not link them.
# Later on, this object library will be linked to our target library
add_library(bof2d-objects OBJECT
  ${PUBLIC_INCLUDES_FILES}
  ${SOURCES_FILES}
)

# Alias with the namespaced version (consistency)
add_library(ONBINGS::bof2d-objects ALIAS bof2d-objects)

# Includes
target_include_directories(bof2d-objects
  PUBLIC
	#Header path used at build time
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	$<BUILD_INTERFACE:${STB_INCLUDE_DIRS}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>		#for bof2d_export.h install (below) and bof2dversioninfo.h (above)
	#$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}
	#$<BUILD_INTERFACE:${FFMPEG_INCLUDE_DIRS}
	#Header path used when installed
	$<INSTALL_INTERFACE:include>
)

# Link
target_link_libraries(bof2d-objects
  PUBLIC
    #ONBINGS::onbings-another-lib

  PRIVATE
#    Qt5::Widgets
	Freetype::Freetype
    glm::glm
	glad::glad
    yuv
	mediainfo
	ONBINGS::bofstd
	SDL2::SDL2main
    SDL2::SDL2 #SDL2-static

#	${Boost_LIBRARIES}
	${FFMPEG_LIBRARIES}
)
#message("=Boost_INCLUDE_DIRS==============>"${Boost_INCLUDE_DIRS})
#message("=Boost_LIBRARIES==============>"${Boost_LIBRARIES})

if(WIN32)
target_link_libraries(bof2d-objects
  PUBLIC
    glfw
#	Ws2_32
#	Iphlpapi
#	Winmm
#	Rpcrt4

  PRIVATE
#    Qt5::Widgets
)
else()
if(NOT "${ANDROID_PLATFORM}")
else()
target_link_libraries(bof2d-objects
  PUBLIC
    glfw
	)
endif()	
endif()	

# Defines
#use external libfmt lib instead of the bundled one
#disable exception in spd log and replace thme by assert(false) to avoid try around every sprintf
target_compile_definitions(bof2d-objects
  PUBLIC
    -DBOF2D_BUILD_MEDIAINFO_AS_LIBRARY=1	#MediaInfo dll is a shame -> use only the static lib version

  PRIVATE
    -Dbof2d_EXPORTS=1	#in lower case to define the SYMBOL BOFSTD_EXPORT (upper case)in bof2d_export.h

#	-DCRT_SECURE_NO_WARNINGS
#	-DCRT_NONSTDC_NO_DEPRECATE
#	-DWINSOCK_DEPRECATED_NO_WARNINGS
)
if(NOT "${ANDROID_PLATFORM}")
target_compile_definitions(bof2d-objects
  PRIVATE
	-DBOF2D_STATIC_DEFINE
)
endif()

message("=BOF2D_VLD==============> " ${BOF2D_VLD})
if (${BOF2D_VLD})
message("===============> Activate Visual Leak Detector")
target_compile_definitions(bof2d-objects
  PRIVATE
	-DBOF2D_VLD
)
endif()

# Features (We need at least C++11 support)
target_compile_features(bof2d-objects
  PUBLIC
    cxx_std_14
)

# ====================
# == CREATE LIBRARY ==
# ====================

# Now create the "real" library
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Dummy.cpp)

# Make sure we have at least one file
add_library(bof2d
  ${CMAKE_CURRENT_BINARY_DIR}/Dummy.cpp
)

# Link with object library
target_link_libraries(bof2d
  PUBLIC
    ONBINGS::bof2d-objects
#PRIVATE 
#	fmt::fmt   # Or use the header-only version   target_link_libraries(main PRIVATE fmt::fmt-header-only)
)

# Create alias
add_library(ONBINGS::bof2d ALIAS bof2d)

# ================
# == VERSIONING ==
# ================

# Set version
if(BOFSTD_ENABLE_VERSIONING)
  # Under Linux this function will create the
  # library file with the full version name
  # and the proper symlinks.
  # e.g.
  #  libbof2d.so.1.0.1
  #  libbof2d.so.1 --> libbof2d.so.1.0.1
  set_target_properties(bof2d PROPERTIES
    VERSION   ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
  )
endif()

# ====================
# == API management ==
# ====================

include(GenerateExportHeader)

# This CMake function generates a header file (i.e. bof2d_export.h)
# that contains all the proper 'defines' to import/export symbols
# (e.g. BOFSTD_EXPORT) for shared libraries. When building as static
# those macros are simply blank. This file will be by default generated
# into ${CMAKE_CURRENT_BINARY_DIR} (see target_include_directories above)
generate_export_header(bof2d)

# ==================
# == INSTALLATION ==
# ==================

if(NOT APPLE)

  # Fix RPATH upon installation by replacing
  # the absolute RPATH generated by the compiler
  # by a keyword that basically says 'look for my
  # dependencies in the directory I'm currently
  # installed'.
  set_target_properties(bof2d PROPERTIES
    INSTALL_RPATH $ORIGIN
  )
endif()

# Install the target runtime
if(NOT "${ANDROID_PLATFORM}")  
install(
  TARGETS  bof2d bof2d-objects
  EXPORT   bof2d
  RUNTIME  DESTINATION        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}     # For binaries and dll
           COMPONENT          bof2d-runtime
  LIBRARY  DESTINATION        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}     # For shared libraries
           COMPONENT          bof2d-runtime
           NAMELINK_COMPONENT bof2d-devel
  ARCHIVE  DESTINATION        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}     # For static libraries
           COMPONENT          bof2d-devel
)
else()		
install(
  TARGETS  bof2d bof2d-objects
  EXPORT   bof2d
  RUNTIME  DESTINATION        ${CMAKE_INSTALL_BINDIR}     # For binaries and dll
           COMPONENT          bof2d-runtime
  LIBRARY  DESTINATION        ${CMAKE_INSTALL_LIBDIR}     # For shared libraries
           COMPONENT          bof2d-runtime
           NAMELINK_COMPONENT bof2d-devel
  ARCHIVE  DESTINATION        ${CMAKE_INSTALL_LIBDIR}     # For static libraries
           COMPONENT          bof2d-devel
)
endif()

# Install the public headers (don't forget the generated export directives)
install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT   bof2d-devel
)
install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/bof2d_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT   bof2d-devel
)
# install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/fmt-5.3.0/include/fmt/.		
# 		 DESTINATION include/fmt-5.3.0
#          COMPONENT   bof2d-devel
# )
# install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third/spdlog-1.3.1/include/.			
#          DESTINATION include/spdlog-1.3.1		
#          COMPONENT   bof2d-devel
# )

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/help/html/. DESTINATION doc COMPONENT doc OPTIONAL)

set_target_properties(bof2d PROPERTIES FOLDER Lib)

