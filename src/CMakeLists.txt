cmake_minimum_required(VERSION 3.5)

include_directories(src)
find_package(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Math DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Infra DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/DWT DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Filter DIR_PROJECT_SRC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -std=c++11 -O0")
set(CMAKE_BUILD_TYPE Debug)

add_library(isp_src ${DIR_PROJECT_SRC})