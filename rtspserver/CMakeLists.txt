PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS  "-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(src/)

AUX_SOURCE_DIRECTORY(.  RTSPSVR_SRV)
AUX_SOURCE_DIRECTORY(src/base  RTSPSVR_SRV)
AUX_SOURCE_DIRECTORY(src/net  RTSPSVR_SRV)
AUX_SOURCE_DIRECTORY(src/net/poller  RTSPSVR_SRV)

ADD_LIBRARY(rtspserver ${RTSPSVR_SRV})

