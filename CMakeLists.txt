PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

#SET(CMAKE_C_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-gcc)

#SET(CMAKE_CXX_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-g++)

#export CC=arm-himix200-linux-gcc
#export CXX=arm-himix200-linux-g++

SET(BOARD_TYPE "IPC")

#SET(WM_BUILD_TYPE "Official")
SET(WM_BUILD_TYPE "factory")
SET(SDK_VER "2v0")

SET(CMAKE_CXX_FLAGS  "-g -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

#include_directories(${CMAKE_SOURCE_DIR}/hihopesdk/business/facerecognition)
if("${SDK_VER}" STREQUAL "1v0")
include_directories(${CMAKE_SOURCE_DIR}/hihopesdk/sdk1v0/include)
SET(sdk_libpath ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/sdk1v0)
else()
include_directories(${CMAKE_SOURCE_DIR}/hihopesdk/sdk2v0/include)
SET(sdk_libpath ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/sdk2v0)
endif()

include_directories(${CMAKE_SOURCE_DIR}/rtspserver/src/)

include_directories(${CMAKE_SOURCE_DIR}/tcp_cmd)
include_directories(${CMAKE_SOURCE_DIR}/cloudserver/src)
include_directories(${CMAKE_SOURCE_DIR}/cloudserver/mqtt_client/src)
include_directories(${CMAKE_SOURCE_DIR}/rs485_port)
add_definitions(-Darm)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)


#SET(CMAKE_C_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-gcc)

#SET(CMAKE_CXX_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-g++)

ADD_SUBDIRECTORY(hihopesdk)
ADD_SUBDIRECTORY(rtspserver)
ADD_SUBDIRECTORY(tcp_cmd)
ADD_SUBDIRECTORY(cloudserver)
ADD_SUBDIRECTORY(rs485_port)

link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/lib)
link_directories(${sdk_libpath}/)
link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/opencv/lib_arm)

if("${WM_BUILD_TYPE}" STREQUAL "factory")
message("factory BUILD ,only for camtest!")
add_definitions(-DBUILD_FACTORY_TEST_APP)

add_executable(aipc src/main.cpp)
target_link_libraries( aipc hihopesdk rtspserver cloudserver rs485 tcp_cmd -ldl -lpthread)
else()
message("Official BUILD !")
add_executable(aipc src/main.cpp)
target_link_libraries( aipc hihopesdk rtspserver cloudserver rs485 tcp_cmd -ldl -lpthread pola gomp)
endif()


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
#-lmpi
#-lisp
)


