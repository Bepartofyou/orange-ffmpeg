cmake_minimum_required(VERSION 3.0)
project(hello)

if(WIN32)
  set(CMAKE_FFMPEG_LIBS_DIR ${CMAKE_SOURCE_DIR}/../../ffmpeg-libs/windll/ffmpeg/)
  set(CMAKE_LIBYUV_LIBS_DIR ${CMAKE_SOURCE_DIR}/../../ffmpeg-libs/windll/libyuv/)

  SET (CMAKE_EXE_LINKER_FLAGS  /SAFESEH:NO)
  SET (CMAKE_SHARED_LINKER_FLAGS /SAFESEH:NO)
  SET (CMAKE_MODULE_LINKER_FLAGS  /SAFESEH:NO)
else(WIN32)
  message(STATUS "linux ffmpeg lib dir")
endif(WIN32)

add_definitions(-D__STDC_CONSTANT_MACROS -D_CRT_SECURE_NO_WARNINGS)

# include   -I
include_directories(
    ${CMAKE_FFMPEG_LIBS_DIR}/include 
    ${CMAKE_LIBYUV_LIBS_DIR}/include 
    )
    
# library   -L
link_directories(
  ${CMAKE_FFMPEG_LIBS_DIR}/lib
  ${CMAKE_LIBYUV_LIBS_DIR}/lib
  )

set(SRC_LIST

ffscale.h
ffscale.cpp
hello.cpp
)

add_executable(hello ${SRC_LIST})
target_link_libraries(hello yuv avcodec avdevice avfilter avformat avutil postproc swresample swscale)

if(WIN32)
  add_custom_command(TARGET hello PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_FFMPEG_LIBS_DIR}/bin $<TARGET_FILE_DIR:hello>)
endif(WIN32)