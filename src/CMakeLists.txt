cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)
find_package(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(tomlplusplus 3.4.0)
include_directories(/usr/local/include/toml++)


aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Algos/Math DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Algos/Infra DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Algos/DWT DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Algos/StarTetrix DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Algos/Filter DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Algos/Color DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/ThirdParty DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Demo DIR_PROJECT_SRC)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/ISP_COMPRESSION DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/ISP_VIN DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/ISP_CCM DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/DUMMY DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Modules/COMMON DIR_PROJECT_SRC)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Pipeline DIR_PROJECT_SRC)



set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} -std=c++17 -O3 -fsanitize=address")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
set(CMAKE_BUILD_TYPE Debug)

add_library(isp_src ${DIR_PROJECT_SRC})