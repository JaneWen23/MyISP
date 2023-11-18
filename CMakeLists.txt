cmake_minimum_required(VERSION 3.5)
 
project(MYISP LANGUAGES CXX)

include_directories(src)
add_subdirectory(src)

aux_source_directory(. SRC_LIST)

find_package(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(MyIspApp "top.cpp")
target_link_libraries(MyIspApp isp_src ${OpenCV_LIBS})

#find_package(OpenMP)
#if(OpenMP_CXX_FOUND)
#    message("OPENMP FOUND")
#endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -std=c++11 -O0")
set(CMAKE_BUILD_TYPE Debug)