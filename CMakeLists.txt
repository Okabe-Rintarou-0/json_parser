cmake_minimum_required(VERSION 3.24)
project(json_parser)

set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(./inc)
aux_source_directory(./src SRC_LIST)
add_executable(json_parser main.cpp ${SRC_LIST} inc/json.h src/json.cpp inc/utils.h)
