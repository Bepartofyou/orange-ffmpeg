cmake_minimum_required(VERSION 3.0)
project(hello)

if(WIN32)
	set(CMAKE_FFMPEG_LIBS_DIR ${CMAKE_SOURCE_DIR}/../../ffmpeg-libs/windll/ffmpeg/)
	set(CMAKE_LIBYUV_LIBS_DIR ${CMAKE_SOURCE_DIR}/../../ffmpeg-libs/windll/libyuv/)

	SET (CMAKE_EXE_LINKER_FLAGS  /SAFESEH:NO)
	SET (CMAKE_SHARED_LINKER_FLAGS /SAFESEH:NO)
	SET (CMAKE_MODULE_LINKER_FLAGS  /SAFESEH:NO)
else(WIN32)
	set(CMAKE_FFMPEG_LIBS_DIR ${CMAKE_SOURCE_DIR}/../../ffmpeg-libs/static/ffmpeg/)
	set(CMAKE_LIBYUV_LIBS_DIR ${CMAKE_SOURCE_DIR}/../../ffmpeg-libs/static/libyuv/)
	set(CMAKE_FDKAAC_LIBS_DIR ${CMAKE_SOURCE_DIR}/../../ffmpeg-libs/static/fdk-aac/)
	set(CMAKE_X264_LIBS_DIR ${CMAKE_SOURCE_DIR}/../../ffmpeg-libs/static/x264/)
endif(WIN32)

add_definitions(-std=c++0x -g -Wall
-D__STDC_CONSTANT_MACROS 
-D__STDC_FORMAT_MACROS
-D_CRT_SECURE_NO_WARNINGS
)

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

if(UNIX)
# include   -I
include_directories(
	${CMAKE_FDKAAC_LIBS_DIR}/include 
	${CMAKE_X264_LIBS_DIR}/include 
	)

# library   -L
link_directories(
	${CMAKE_FDKAAC_LIBS_DIR}/lib
	${CMAKE_X264_LIBS_DIR}/lib
	)

endif(UNIX)

set(SRC_LIST

	ffmuxfile.h
	ffmuxfile.cpp
	hello.cpp
	)

add_executable(hello ${SRC_LIST})

if(WIN32)
	target_link_libraries(hello 
		yuv 
		avcodec 
		avdevice 
		avfilter 
		avformat 
		avutil 
		postproc 
		swresample 
		swscale
		)
else(WIN32)
	target_link_libraries(hello 
		yuv
		avformat
		avdevice
		avcodec
		avutil
		avfilter
		postproc
		swresample
		swscale
		fdk-aac 
		x264
		bz2
		dl
		rt
		m
		z
		c
		)
endif(WIN32)

if(WIN32)
	add_custom_command(TARGET hello PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_FFMPEG_LIBS_DIR}/bin $<TARGET_FILE_DIR:hello>)
endif(WIN32)
