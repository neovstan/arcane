cmake_minimum_required(VERSION 3.16)

project(samp_utils)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} "include/${PROJECT_NAME}/samp_utils.h" "src/${PROJECT_NAME}/samp_utils.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE include/${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} sampapi)