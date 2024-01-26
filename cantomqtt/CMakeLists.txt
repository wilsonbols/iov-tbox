cmake_minimum_required(VERSION 3.28)

#find_package(eclipse-paho-mqtt-c REQUIRED)
project(cantomqtt C)

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


