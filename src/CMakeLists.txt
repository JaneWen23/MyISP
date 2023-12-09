cmake_minimum_required(VERSION 3.5)

include_directories(src)
find_package(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/Math DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/Infra DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/DWT DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/StarTetrix DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/Compression DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/Filter DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/CSC DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ThirdParty DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Demo DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Pipeline DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/PipelineModules DIR_PROJECT_SRC)


set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -std=c++17 -O3 -fsanitize=address")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
set(CMAKE_BUILD_TYPE Debug)

add_library(isp_src ${DIR_PROJECT_SRC})