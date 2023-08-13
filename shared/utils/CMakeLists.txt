cmake_minimum_required(VERSION 3.16)

project(utils)

add_library(utils STATIC
        include/utils/utils.h
        include/utils/randomizer.hpp
        src/utils/utils.cpp
)

target_include_directories(utils PRIVATE include/utils PUBLIC include)
target_link_libraries(utils PRIVATE infoware data_representation)

set_target_properties(utils PROPERTIES CXX_STANDARD 17)
