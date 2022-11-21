cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
project(EVEN_ODD)
include_directories(".")
add_library(evenoddlib "")
target_sources(evenoddlib
    PRIVATE
    "util.h"
    "log.h"
    "log.cpp"
    "util/ThreadPool.h"

    "decoding.h"
    "decoding.cpp"
    "encoding.h"
    "encoding.cpp"
    "repair.h"
    "repair.cpp"

    
)
target_link_libraries(evenoddlib "pthread")
add_executable(evenodd "evenodd.cpp")
target_link_libraries(evenodd evenoddlib)

add_executable(time_check time_check.cpp)
target_link_libraries(time_check evenoddlib)
