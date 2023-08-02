cmake_minimum_required(VERSION 3.15)
project(configuration)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                      $<INSTALL_INTERFACE:include/${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR})

find_package(nlohmann_json)
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)