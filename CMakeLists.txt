cmake_minimum_required(VERSION 3.5)
 
project(MYISP LANGUAGES CXX)

include_directories(src)
add_subdirectory(src)

aux_source_directory(. SRC_LIST)

add_executable(MyIspApp "top.cpp")
target_link_libraries(MyIspApp isp_src)

