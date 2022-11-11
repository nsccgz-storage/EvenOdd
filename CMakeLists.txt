cmake_minimum_required(VERSION 3.18.4)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
project(EVEN_ODD)

add_library(evenoddlib "")
target_sources(evenoddlib
    PRIVATE
    "decoding.h"
    "decoding.cpp"
    "encoding.h"
    "encoding.cpp"

    "log.h"
    "log.cpp"
)

add_executable(evenodd "evenodd.cpp")
target_link_libraries(evenodd evenoddlib)
