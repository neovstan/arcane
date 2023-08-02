cmake_minimum_required(VERSION 3.16)

project(data_representation)

set(CMAKE_CXX_STANDARD 17)
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_MSVC_STATIC_RT TRUE)

add_library(${PROJECT_NAME}
    include/${PROJECT_NAME}/${PROJECT_NAME}.h
    src/${PROJECT_NAME}/${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include/${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(OpenSSL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)