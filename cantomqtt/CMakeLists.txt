cmake_minimum_required(VERSION 3.12)

#find_package(eclipse-paho-mqtt-c REQUIRED)
project(cantomqtt C)

#compile with arm 
SET(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
# Name of the target platform
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)
# Version of the system
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_FLAGS "-march=armv5te")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
set(CMAKE_C_STANDARD 11)

LINK_LIBRARIES(m pthread)

file(REMOVE_RECURSE "cmake-build-debug")


#add_executable(cantomqtt writecan.c common/conversion.c)

add_executable(cantomqtt main.c common/conversion.c)

find_library(PAHO_MQTT3C_LIB paho-mqtt3c PATHS /usr/local/lib)
target_link_libraries(cantomqtt ${PAHO_MQTT3C_LIB})

#加载第三方依赖库jansson
find_library(PAHO_JANSSON_LIB jansson PATHS /usr/local/lib)
target_link_libraries(cantomqtt ${PAHO_JANSSON_LIB})


