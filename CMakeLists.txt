cmake_minimum_required(VERSION 3.12)
project(gtest_c_cpp_demo C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 不再使用LINK_LIBRARIES全局链接库
#LINK_LIBRARIES(m pthread)

#file(REMOVE_RECURSE "build")

# 添加主程序的可执行文件
# add_executable(gtest_c_cpp_demo_main main.cpp add.cpp libs/equal.c cantomqtt/control.c)  # 确保add.cpp/,libs/equal.c被编译进主程序
# target_link_libraries(gtest_c_cpp_demo_main jansson)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR}/_deps/googletest-src/googletest/include)

# Mockcpp https://github.com/sinojelly/mockcpp
# INCLUDE_DIRECTORIES(BEFORE 
#     ${PROJECT_SOURCE_DIR}/3rdparty/mockcpp/include)
#     #${PROJECT_SOURCE_DIR}/3rdparty/boost/include)
# link_directories(${PROJECT_SOURCE_DIR}/3rdparty/mockcpp/lib)


# 添加测试目标
add_executable(tbox_gtest_tests tests/test_tbox.cpp tests/test_mock.cpp cantomqtt/control.c)  # 确保add.cpp和libs/equal.c被编译进测试目标
target_link_libraries(tbox_gtest_tests gtest gtest_main jansson mockcpp)

# 添加覆盖率编译器标志，编译时使用 参数来传递：cmake -DCODE_COVERAGE=ON ..
if(CODE_COVERAGE)   
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

# 注册测试到ctest模块中
enable_testing()
add_test(NAME tbox_gtest_tests COMMAND tbox_gtest_tests)


