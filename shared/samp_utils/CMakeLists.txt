cmake_minimum_required(VERSION 3.16)

project(samp_utils)

set(CMAKE_CXX_STANDARD 17)

add_library(samp_utils
    include/samp_utils/samp_utils.h
    src/samp_utils/samp_utils.cpp
)

target_include_directories(samp_utils PRIVATE include/samp_utils PUBLIC include)

target_link_libraries(samp_utils sampapi)
