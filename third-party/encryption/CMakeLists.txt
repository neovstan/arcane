cmake_minimum_required(VERSION 3.16)

# The project related to the client library
project(encryption)

find_package(cryptopp CONFIG REQUIRED)

add_library(${PROJECT_NAME} "include/${PROJECT_NAME}/${PROJECT_NAME}.h" "src/${PROJECT_NAME}/${PROJECT_NAME}.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE include/${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE cryptopp::cryptopp)