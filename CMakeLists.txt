cmake_minimum_required(VERSION 3.12)
find_package(eclipse-paho-mqtt-c 1.3.13 REQUIRED)
project(tbox C)

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
set(CMAKE_C_STANDARD 99)

LINK_LIBRARIES(m pthread)

file(REMOVE_RECURSE "cmake-build-debug")

add_executable(IoV-Edge-CClient main.c)
find_library(PAHO_MQTT3C_LIB paho-mqtt3c PATHS /usr/local/lib)
target_link_libraries(IoV-Edge-CClient ${PAHO_MQTT3C_LIB})


