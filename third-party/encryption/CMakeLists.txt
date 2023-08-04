cmake_minimum_required(VERSION 3.16)

project(encryption)

find_package(cryptopp CONFIG REQUIRED)

add_library(encryption
    include/encryption/encryption.h
    src/encryption/encryption.cpp
)

target_include_directories(encryption PRIVATE include/encryption PUBLIC include)

target_link_libraries(encryption PRIVATE cryptopp::cryptopp)
