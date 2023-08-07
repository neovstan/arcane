cmake_minimum_required(VERSION 3.16)

project(data_representation)

set(CMAKE_CXX_STANDARD 17)
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_MSVC_STATIC_RT TRUE)

find_package(OpenSSL REQUIRED)

add_library(data_representation
    include/data_representation/data_representation.h
    src/data_representation/data_representation.cpp
)

target_include_directories(data_representation PRIVATE include/data_representation PUBLIC include)

target_link_libraries(data_representation PRIVATE OpenSSL::SSL OpenSSL::Crypto)
