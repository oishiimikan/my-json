cmake_minimum_required(VERSION 3.0)
project(my-json)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror -std=c++11 -Wno-deprecated -Wno-unused-function")

add_library(my-json SHARED src/my_json.cc)

add_executable(main main.cc)
add_dependencies(main my-json)
target_link_libraries(main my-json)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)