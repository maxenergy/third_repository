PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS  "-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

if("${SDK_VER}" STREQUAL "1v0")
include_directories(${CMAKE_SOURCE_DIR}/sdk1v0/include)
add_definitions(-DBUILD_SDK1V0)
else()
include_directories(${CMAKE_SOURCE_DIR}/sdk2v0/include)
add_definitions(-DBUILD_SDK2V0)
add_definitions(-DZQ_DETECT_YOLOV3)
endif()

AUX_SOURCE_DIRECTORY(base/core/src/ai  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/camera  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/operator  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/queue  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/reflect  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/thread  HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(base/core/src/thread  HIHOPE_SRCS)

AUX_SOURCE_DIRECTORY(hardware/dummy/src HIHOPE_SRCS)

AUX_SOURCE_DIRECTORY(hardware/hi3516/src/adapter HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(hardware/hi3516/src/mpp HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(hardware/hi3516/src/nnie HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(hardware/hi3516/src/yolov3 HIHOPE_SRCS)
if("${SDK_VER}" STREQUAL "1v0")
AUX_SOURCE_DIRECTORY(hardware/hi3516/src/common/sdk1v0 HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(hardware/hi3516/src/mpp/sdk1v0 HIHOPE_SRCS)
SET(sdk_libpath ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/sdk1v0)
else()
AUX_SOURCE_DIRECTORY(hardware/hi3516/src/common/sdk2v0 HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(hardware/hi3516/src/mpp/sdk2v0 HIHOPE_SRCS)
SET(sdk_libpath ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/sdk2v0)
endif()

if("${BOARD_TYPE}" STREQUAL "IPC")
    message("BUILD IPC BOARD!")
    AUX_SOURCE_DIRECTORY(hardware/hi3516/src/mpp/board_ipc HIHOPE_SRCS)
else()
    message("BUILD Integrated_machine BOARD!")
    add_definitions(-DIR_CAMERA)
    AUX_SOURCE_DIRECTORY(hardware/hi3516/src/mpp/board_Integrated_machine HIHOPE_SRCS)
endif()


if("${WM_BUILD_TYPE}" STREQUAL "factory")
message("factory BUILD ,only for camtest!")
add_definitions(-DBUILD_FACTORY_TEST_APP)
else()
message("Official BUILD !")
endif()


AUX_SOURCE_DIRECTORY(hardware/hi3516/src/utils HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(business/facerecognition/src HIHOPE_SRCS)
AUX_SOURCE_DIRECTORY(datamanager/db_facerecognition/src HIHOPE_SRCS)

add_library(SQLiteCpp STATIC IMPORTED)
set_property(TARGET SQLiteCpp PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/sqlitecpp/lib_arm/libSQLiteCpp.a)

add_library(sqlite3 STATIC IMPORTED)
set_property(TARGET sqlite3 PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/sqlite3/lib_arm/libsqlite3.a)


add_library(libdnvqe STATIC IMPORTED)
set_property(TARGET libdnvqe PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libdnvqe.a)


add_library(libhdmi STATIC IMPORTED)
set_property(TARGET libhdmi PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libhdmi.a)


add_library(lib_hiawb STATIC IMPORTED)
set_property(TARGET lib_hiawb PROPERTY IMPORTED_LOCATION ${sdk_libpath}/lib_hiawb.a)

add_library(lib_hiae STATIC IMPORTED)
set_property(TARGET lib_hiae PROPERTY IMPORTED_LOCATION ${sdk_libpath}/lib_hiae.a)

#add_library(libmpi STATIC IMPORTED)
#set_property(TARGET libmpi PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libmpi.a)

add_library(libsns_imx327 STATIC IMPORTED)
set_property(TARGET libsns_imx327 PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_imx327.a)


add_library(lib_hiawb_natura STATIC IMPORTED)
set_property(TARGET lib_hiawb_natura PROPERTY IMPORTED_LOCATION ${sdk_libpath}/lib_hiawb_natura.a)

add_library(libhi_cipher STATIC IMPORTED)
set_property(TARGET libhi_cipher PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libhi_cipher.a)


add_library(lib_hidehaze STATIC IMPORTED)
set_property(TARGET lib_hidehaze PROPERTY IMPORTED_LOCATION ${sdk_libpath}/lib_hidehaze.a)


add_library(lib_hidrc STATIC IMPORTED)
set_property(TARGET lib_hidrc PROPERTY IMPORTED_LOCATION ${sdk_libpath}/lib_hidrc.a)

add_library(libhifisheyecalibrate STATIC IMPORTED)
set_property(TARGET libhifisheyecalibrate PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libhifisheyecalibrate.a)


add_library(lib_hildci STATIC IMPORTED)
set_property(TARGET lib_hildci PROPERTY IMPORTED_LOCATION ${sdk_libpath}/lib_hildci.a)

#add_library(libisp STATIC IMPORTED)
#set_property(TARGET libisp PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/hihopesdk/libs/lib3516/lib/libisp.a)


add_library(libive STATIC IMPORTED)
set_property(TARGET libive PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libive.a)

add_library(libmd STATIC IMPORTED)
set_property(TARGET libmd PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libmd.a)

add_library(libsecurec STATIC IMPORTED)
set_property(TARGET libsecurec PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsecurec.a)


add_library(libsvpruntime STATIC IMPORTED)
set_property(TARGET libsvpruntime PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsvpruntime.a)


add_library(libtde STATIC IMPORTED)
set_property(TARGET libtde PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libtde.a)



add_library(libupvqe STATIC IMPORTED)
set_property(TARGET libupvqe PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libupvqe.a)


add_library(libVoiceEngine STATIC IMPORTED)
set_property(TARGET libVoiceEngine PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libVoiceEngine.a)


add_library(libsns_imx307_2l STATIC IMPORTED)
set_property(TARGET libsns_imx307_2l PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_imx307_2l.a)

add_library(libsns_imx307 STATIC IMPORTED)
set_property(TARGET libsns_imx307 PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_imx307.a)

add_library(libsns_imx327_2l STATIC IMPORTED)
set_property(TARGET libsns_imx327_2l PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_imx327_2l.a)

add_library(libsns_imx335 STATIC IMPORTED)
set_property(TARGET libsns_imx335 PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_imx335.a)

add_library(libsns_imx377 STATIC IMPORTED)
set_property(TARGET libsns_imx377 PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_imx377.a)

add_library(libsns_imx458 STATIC IMPORTED)
set_property(TARGET libsns_imx458 PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_imx458.a)


add_library(libsns_mn34220 STATIC IMPORTED)
set_property(TARGET libsns_mn34220 PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_mn34220.a)


add_library(libsns_os05a STATIC IMPORTED)
set_property(TARGET libsns_os05a PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_os05a.a)


add_library(libsns_os05a_2l STATIC IMPORTED)
set_property(TARGET libsns_os05a_2l PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libsns_os05a_2l.a)



add_library(libmpi STATIC IMPORTED)
set_property(TARGET libmpi PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libmpi.a)


add_library(libisp STATIC IMPORTED)
set_property(TARGET libisp PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libisp.a)


add_library(libnnie STATIC IMPORTED)
set_property(TARGET libnnie PROPERTY IMPORTED_LOCATION ${sdk_libpath}/libnnie.a)


if("${SDK_VER}" STREQUAL "2v0")
add_library(lib_hicalcflicker STATIC IMPORTED)
set_property(TARGET lib_hicalcflicker PROPERTY IMPORTED_LOCATION ${sdk_libpath}/lib_hicalcflicker.a)
add_library(Hi3516DV3_Nr STATIC IMPORTED)
set_property(TARGET Hi3516DV3_Nr PROPERTY IMPORTED_LOCATION ${sdk_libpath}/Hi3516DV3_Nr.a)
endif()




if("${BOARD_TYPE}" STREQUAL "IPC")
    message("BUILD IMX327 BOARD!")
    #add_definitions(-DSENSOR0_TYPE=SONY_IMX327_2L_MIPI_2M_30FPS_12BIT)
    #add_definitions(-DSENSOR1_TYPE=SONY_IMX327_2L_MIPI_2M_30FPS_12BIT)
    add_definitions(-DSENSOR0_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
    add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
    #add_definitions(-DSENSOR0_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1)
    #add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1)
else()
    message("BUILD IMX327 BOARD!")
	add_definitions(-DSENSOR0_TYPE=SONY_IMX327_2L_MIPI_2M_30FPS_12BIT)
	add_definitions(-DSENSOR1_TYPE=SONY_IMX327_2L_MIPI_2M_30FPS_12BIT)
endif()

add_definitions(-Darm)

link_directories(${PROJECT_SOURCE_DIR}/hihopesdk/libs/lib3516/lib)

ADD_LIBRARY(hihopesdk ${HIHOPE_SRCS})
if("${SDK_VER}" STREQUAL "1v0")
target_link_libraries(hihopesdk SQLiteCpp  sqlite3 libisp)
else()
target_link_libraries(hihopesdk SQLiteCpp  sqlite3 libisp lib_hicalcflicker Hi3516DV3_Nr)
endif()
target_link_libraries(hihopesdk  libmpi libdnvqe libhdmi lib_hiae lib_hiawb libsns_imx327 lib_hiawb_natura libhi_cipher lib_hidehaze lib_hidrc libhifisheyecalibrate lib_hildci libive libmd libsecurec libsvpruntime libtde libupvqe libVoiceEngine
libsns_imx307_2l libsns_imx307 libsns_imx327_2l libsns_imx327 libsns_imx335  libsns_imx458  libnnie
  )
#libsns_imx377
#libsns_mn34220
#libsns_os05a_2l
#libsns_os05a
#target_link_libraries(hihopesdk mpi isp)

