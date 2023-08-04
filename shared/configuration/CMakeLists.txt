cmake_minimum_required(VERSION 3.15)
project(configuration)

add_library(configuration INTERFACE)

target_include_directories(configuration INTERFACE include)

find_package(nlohmann_json)
target_link_libraries(configuration INTERFACE nlohmann_json::nlohmann_json)
