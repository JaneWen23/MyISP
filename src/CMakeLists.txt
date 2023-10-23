include_directories(src)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/DWT DIR_PROJECT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Infra DIR_PROJECT_SRC)

add_library(isp_src ${DIR_PROJECT_SRC})