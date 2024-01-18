cmake_minimum_required(VERSION 3.12)
find_package(eclipse-paho-mqtt-c 1.3.13 REQUIRED)
project(IoV-Edge-CClient C)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)
set(CMAKE_C_STANDARD 99)

LINK_LIBRARIES(m pthread)

file(REMOVE_RECURSE "cmake-build-debug")

add_executable(IoV-Edge-CClient main.c)
find_library(PAHO_MQTT3C_LIB paho-mqtt3c PATHS /usr/local/lib)
target_link_libraries(IoV-Edge-CClient ${PAHO_MQTT3C_LIB})


