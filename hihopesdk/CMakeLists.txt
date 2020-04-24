PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS  "-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

include_directories(${CMAKE_SOURCE_DIR}/include)

AUX_SOURCE_DIRECTORY(base/core/src/ai  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/camera  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/operator  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/queue  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/reflect  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/thread  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/thread  HIHOPE_SRCS)

AUX_SOURCE_DIRECTORY(hardware/dummy/src HIHOPE_SRCS)

AUX_SOURCE_DIRECTORY(hardware/hi3516/src/adapter HIHOPE_SRCS)

AUX_SOURCE_DIRECTORY(hardware/hi3516/src/common HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(hardware/hi3516/src/mpp HIHOPE_SRCS)

if("${BOARD_TYPE}" STREQUAL "IPC")
    message("BUILD IPC BOARD!")
    AUX_SOURCE_DIRECTORY(hardware/hi3516/src/mpp/board_ipc HIHOPE_SRCS)
else()
    message("BUILD Integrated_machine BOARD!")
    add_definitions(-DIR_CAMERA)
    AUX_SOURCE_DIRECTORY(hardware/hi3516/src/mpp/board_Integrated_machine HIHOPE_SRCS)
endif()


AUX_SOURCE_DIRECTORY(hardware/hi3516/src/utils HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(business/facerecognition/src HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(datamanager/db_facerecognition/src HIHOPE_SRCS)

add_library(SQLiteCpp STATIC IMPORTED)
set_property(TARGET SQLiteCpp PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/sqlitecpp/lib_arm/libSQLiteCpp.a)

add_library(sqlite3 STATIC IMPORTED)
set_property(TARGET sqlite3 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/sqlite3/lib_arm/libsqlite3.a)


add_library(libdnvqe STATIC IMPORTED)
set_property(TARGET libdnvqe PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libdnvqe.a)


add_library(libhdmi STATIC IMPORTED)
set_property(TARGET libhdmi PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libhdmi.a)


add_library(lib_hiae STATIC IMPORTED)
set_property(TARGET lib_hiae PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/lib_hiae.a)


#add_library(libmpi STATIC IMPORTED)
#set_property(TARGET libmpi PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libmpi.a)

add_library(libsns_imx327 STATIC IMPORTED)
set_property(TARGET libsns_imx327 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_imx327.a)


add_library(lib_hiawb_natura STATIC IMPORTED)
set_property(TARGET lib_hiawb_natura PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/lib_hiawb_natura.a)

add_library(libhi_cipher STATIC IMPORTED)
set_property(TARGET libhi_cipher PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libhi_cipher.a)


add_library(lib_hidehaze STATIC IMPORTED)
set_property(TARGET lib_hidehaze PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/lib_hidehaze.a)


add_library(lib_hidrc STATIC IMPORTED)
set_property(TARGET lib_hidrc PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/lib_hidrc.a)

add_library(libhifisheyecalibrate STATIC IMPORTED)
set_property(TARGET libhifisheyecalibrate PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libhifisheyecalibrate.a)


add_library(lib_hildci STATIC IMPORTED)
set_property(TARGET lib_hildci PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/lib_hildci.a)

#add_library(libisp STATIC IMPORTED)
#set_property(TARGET libisp PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libisp.a)


add_library(libive STATIC IMPORTED)
set_property(TARGET libive PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libive.a)

add_library(libmd STATIC IMPORTED)
set_property(TARGET libmd PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libmd.a)

add_library(libsecurec STATIC IMPORTED)
set_property(TARGET libsecurec PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsecurec.a)


add_library(libsvpruntime STATIC IMPORTED)
set_property(TARGET libsvpruntime PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsvpruntime.a)


add_library(libtde STATIC IMPORTED)
set_property(TARGET libtde PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libtde.a)



add_library(libupvqe STATIC IMPORTED)
set_property(TARGET libupvqe PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libupvqe.a)


add_library(libVoiceEngine STATIC IMPORTED)
set_property(TARGET libVoiceEngine PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libVoiceEngine.a)


add_library(libsns_imx307_2l STATIC IMPORTED)
set_property(TARGET libsns_imx307_2l PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_imx307_2l.a)

add_library(libsns_imx307 STATIC IMPORTED)
set_property(TARGET libsns_imx307 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_imx307.a)

add_library(libsns_imx327_2l STATIC IMPORTED)
set_property(TARGET libsns_imx327_2l PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_imx327_2l.a)

add_library(libsns_imx335 STATIC IMPORTED)
set_property(TARGET libsns_imx335 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_imx335.a)

add_library(libsns_imx377 STATIC IMPORTED)
set_property(TARGET libsns_imx377 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_imx377.a)

add_library(libsns_imx458 STATIC IMPORTED)
set_property(TARGET libsns_imx458 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_imx458.a)


add_library(libsns_mn34220 STATIC IMPORTED)
set_property(TARGET libsns_mn34220 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_mn34220.a)


add_library(libsns_os05a STATIC IMPORTED)
set_property(TARGET libsns_os05a PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_os05a.a)


add_library(libsns_os05a_2l STATIC IMPORTED)
set_property(TARGET libsns_os05a_2l PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libsns_os05a_2l.a)


add_definitions(-Darm)
add_definitions(-DSENSOR0_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)


link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/lib3516/lib)

ADD_LIBRARY(hihopesdk ${HIHOPE_SRCS})

target_link_libraries(hihopesdk SQLiteCpp  sqlite3 libdnvqe libhdmi lib_hiae libsns_imx327 lib_hiawb_natura libhi_cipher lib_hidehaze lib_hidrc
libhifisheyecalibrate lib_hildci libive libmd libsecurec libsvpruntime libtde libupvqe libVoiceEngine
libsns_imx307_2l libsns_imx307 libsns_imx327_2l libsns_imx327 libsns_imx335 libsns_imx377 libsns_imx458 libsns_mn34220
libsns_os05a_2l libsns_os05a)

#target_link_libraries(hihopesdk mpi isp)

