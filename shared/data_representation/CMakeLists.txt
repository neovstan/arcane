cmake_minimum_required(VERSION 3.16)

project(data_representation)

add_library(data_representation STATIC
        include/data_representation/data_representation.h
        src/data_representation/data_representation.cpp
)

target_include_directories(data_representation PRIVATE include/data_representation PUBLIC include)

target_link_libraries(data_representation PRIVATE cryptopp::cryptopp)

set_target_properties(data_representation PROPERTIES CXX_STANDARD 17)
