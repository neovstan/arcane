cmake_minimum_required(VERSION 3.16)

project(winapi_utils)

add_library(winapi_utils
    include/winapi_utils/winapi_utils.h
    src/winapi_utils/winapi_utils.cpp
)

target_include_directories(winapi_utils PRIVATE include/winapi_utils PUBLIC include)

target_link_libraries(winapi_utils PRIVATE utils vmprotect-sdk data_representation)

set_target_properties(winapi_utils PROPERTIES CXX_STANDARD 17)
