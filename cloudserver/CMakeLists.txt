PROJECT(AIPC)
cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS  "-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -fno-aggressive-loop-optimizations -fPIC")

include_directories(src)

include_directories(mqtt_client/src)

AUX_SOURCE_DIRECTORY(src CLOUDSERVER)

AUX_SOURCE_DIRECTORY(mqtt_client/src MQTTCLIENT)

add_library(libmqttc STATIC IMPORTED)
set_property(TARGET libmqttc PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/cloudserver/mqtt_client/lib/libmqttc.a)


ADD_LIBRARY(cloudserver ${CLOUDSERVER} ${MQTTCLIENT})

target_link_libraries(cloudserver libmqttc)