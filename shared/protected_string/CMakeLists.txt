cmake_minimum_required(VERSION 3.21)

project(protected_string)

add_library(protected_string STATIC
        include/protected_string/protected_string.h
        src/protected_string/protected_string.cpp
)

target_link_libraries(protected_string PRIVATE vmprotect-sdk)
target_include_directories(protected_string PRIVATE include/protected_string PUBLIC include)

set_target_properties(protected_string PROPERTIES CXX_STANDARD 11)
