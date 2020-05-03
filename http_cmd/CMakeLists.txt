PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS  "-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(src/)

AUX_SOURCE_DIRECTORY(src  HTTP_SRV)

ADD_LIBRARY(http_cmd ${HTTP_SRV})

