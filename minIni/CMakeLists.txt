PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS  "-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

include_directories(dev)

ADD_LIBRARY(minIni dev/minIni.c)
