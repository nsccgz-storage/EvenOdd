cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
project(EVEN_ODD)
include_directories(".")
include_directories("src")
add_library(evenoddlib "")
target_sources(evenoddlib
    PRIVATE
    "util/util.h"
    "util/log.h"
    "util/log.cpp"
    "util/ThreadPool.h"

    "src/decoding.h"
    "src/decoding.cpp"
    "src/encoding.h"
    "src/encoding.cpp"
    "src/repair.h"
    "src/repair.cpp"
)
target_link_libraries(evenoddlib "pthread")
add_executable(evenodd "src/evenodd.cpp")
target_link_libraries(evenodd evenoddlib)

add_executable(time_check "src/time_check.cpp")
target_link_libraries(time_check evenoddlib)
