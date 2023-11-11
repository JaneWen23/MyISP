cmake_minimum_required(VERSION 3.5)
 
project(MYISP LANGUAGES CXX)

include_directories(src)
add_subdirectory(src)

aux_source_directory(. SRC_LIST)

add_executable(MyIspApp "top.cpp")
target_link_libraries(MyIspApp isp_src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -std=c++11 -O0")
set(CMAKE_BUILD_TYPE Debug)