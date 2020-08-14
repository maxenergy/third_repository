PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

find_package(Qt4 4.8.6 REQUIRED QtCore QtGui QtXml QtNetwork)

#SET(CMAKE_C_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-gcc)

#SET(CMAKE_CXX_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-g++)

#export CC=arm-himix200-linux-gcc
#export CXX=arm-himix200-linux-g++

SET(BOARD_TYPE "LCDBOARD")
#SET(BOARD_TYPE "IPC")
SET(WM_BUILD_TYPE "Official")
#SET(WM_BUILD_TYPE "factory")
SET(SDK_VER "1v0")

SET(CMAKE_CXX_FLAGS  "-g -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

#include_directories(${CMAKE_SOURCE_DIR}/hihopesdk/business/facerecognition)
if("${SDK_VER}" STREQUAL "1v0")
include_directories(${CMAKE_SOURCE_DIR}/hihopesdk/sdk1v0/include)
SET(sdk_libpath ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/sdk1v0)
else()
include_directories(${CMAKE_SOURCE_DIR}/hihopesdk/sdk2v0/include)
SET(sdk_libpath ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/sdk2v0)
endif()

#add_definitions(-DZQ_DETECT_E-BICYCLE)

include_directories(${CMAKE_SOURCE_DIR}/rtspserver/src/)

if("${BOARD_TYPE}" STREQUAL "LCDBOARD")
include_directories(${CMAKE_SOURCE_DIR}/FacialAccessControl/include)
include_directories(${CMAKE_SOURCE_DIR}/FacialAccessControl)
include_directories(/usr/local/Qt-4.8.6/include)
AUX_SOURCE_DIRECTORY(FacialAccessControl  FRG_SRCS)
link_directories(${PROJECT_SOURCE_DIR}/zbar/lib)
SET(qt4_FORMS FacialAccessControl/*.ui)
SET(qt4_RESOURCES FacialAccessControl/img.qrc)
QT4_WRAP_UI(qt4_FORMS_headers ${qt4_FORMS})
QT4_ADD_RESOURCES(qt4_RESOURCES_headers ${qt4_RESOURCES})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
endif()
include_directories(${CMAKE_SOURCE_DIR}/tcp_cmd)
include_directories(${CMAKE_SOURCE_DIR}/cloudserver/src)
include_directories(${CMAKE_SOURCE_DIR}/cloudserver/mqtt_client/src)
include_directories(${CMAKE_SOURCE_DIR}/rs485_port)
include_directories(${CMAKE_SOURCE_DIR}/zbar/include)
include_directories(${CMAKE_SOURCE_DIR}/minIni/dev)
add_definitions(-Darm)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)


#SET(CMAKE_C_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-gcc)

#SET(CMAKE_CXX_COMPILER /media/alik/f54faace-0b8b-4347-99e8-abd0dfe48105/opt/arm-himix200-linux/bin/arm-himix200-linux-g++)

ADD_SUBDIRECTORY(hihopesdk)
ADD_SUBDIRECTORY(rtspserver)
ADD_SUBDIRECTORY(tcp_cmd)
ADD_SUBDIRECTORY(cloudserver)
ADD_SUBDIRECTORY(rs485_port)
ADD_SUBDIRECTORY(minIni)

link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/lib)
link_directories(${sdk_libpath}/)
link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/opencv/lib_arm)
link_directories(${PROJECT_SOURCE_DIR}/zbar/lib)
link_directories(${PROJECT_SOURCE_DIR}/minIni/build)

if("${WM_BUILD_TYPE}" STREQUAL "factory")
message("factory BUILD ,only for camtest!")
add_definitions(-DBUILD_FACTORY_TEST_APP)

if("${BOARD_TYPE}" STREQUAL "IPC")
add_executable(aipc src/main.cpp)
else()
add_executable(aipc ${FRG_SRCS} ${qt4_FORMS_headers} ${qt4_RESOURCES_headers})
TARGET_LINK_LIBRARIES(aipc ${QT_LIBRARIES} minIni cloudserver)
endif()

target_link_libraries( aipc hihopesdk rtspserver cloudserver rs485 tcp_cmd minIni -ldl -lpthread)
else()
message("Official BUILD !")
if("${BOARD_TYPE}" STREQUAL "IPC")
add_executable(aipc src/main.cpp)
else()
add_executable(aipc ${FRG_SRCS} ${qt4_FORMS_headers} ${qt4_RESOURCES_headers})
TARGET_LINK_LIBRARIES(aipc ${QT_LIBRARIES} minIni cloudserver)
endif()
target_link_libraries( aipc hihopesdk rtspserver cloudserver rs485 tcp_cmd minIni -ldl -lpthread pola gomp)
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
-lzbar
#-lmpi
#-lisp
)


