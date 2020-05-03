PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

#SET(CMAKE_C_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-gcc)

#SET(CMAKE_CXX_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-g++)

#export CC=arm-himix200-linux-gcc
#export CXX=arm-himix200-linux-g++

SET(BOARD_TYPE "IPC")

SET(CMAKE_CXX_FLAGS  "-g -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

#include_directories(${CMAKE_SOURCE_DIR}/hihopesdk/business/facerecognition)
include_directories(${CMAKE_SOURCE_DIR}/hihopesdk/include)
include_directories(${CMAKE_SOURCE_DIR}/rtspserver/src/)
include_directories(${CMAKE_SOURCE_DIR}/http_cmd/inc)

add_definitions(-Darm)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)


#SET(CMAKE_C_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-gcc)

#SET(CMAKE_CXX_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-g++)

ADD_SUBDIRECTORY(hihopesdk)
ADD_SUBDIRECTORY(rtspserver)
ADD_SUBDIRECTORY(http_cmd)

link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/lib)
link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/lib3516/lib)
link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/opencv/lib_arm)

add_executable(aipc src/main.cpp)

target_link_libraries( aipc hihopesdk rtspserver http_cmd -ldl -lpthread pola gomp)


target_link_libraries(aipc 
-lopencv_calib3d             
-lopencv_core             
-lopencv_features2d             
-lopencv_flann             
-lopencv_highgui             
-lopencv_imgcodecs             
-lopencv_imgproc             
-lopencv_ml             
-lopencv_objdetect             
-lopencv_photo             
-lopencv_shape             
-lopencv_stitching             
-lopencv_superres             
-lopencv_videoio             
-lopencv_video             
-lopencv_videostab
-lmpi
-lisp
)
