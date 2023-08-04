cmake_minimum_required(VERSION 3.16)

project(winapi_utils)

set(CMAKE_CXX_STANDARD 17)

add_library(winapi_utils
    include/winapi_utils/winapi_utils.h
    src/winapi_utils/winapi_utils.cpp
)

target_include_directories(winapi_utils PRIVATE include/winapi_utils PUBLIC include)

target_link_libraries(winapi_utils PRIVATE vmprotect-sdk data_representation)