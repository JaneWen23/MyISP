cmake_minimum_required(VERSION 3.5)
 
project(MYISP LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)
add_subdirectory(src)

aux_source_directory(. SRC_LIST)

find_package(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(tomlplusplus 3.4.0)
include_directories(/usr/local/include/toml++)

add_executable(MyIspApp "top.cpp")
target_link_libraries(MyIspApp isp_src ${OpenCV_LIBS})

#find_package(OpenMP)
#if(OpenMP_CXX_FOUND)
#    message("OPENMP FOUND")
#endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -std=c++17 -O3 -fsanitize=address -fno-omit-frame-pointer") # AppleClang does not support detect_leaks!!
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
set(CMAKE_BUILD_TYPE Debug)