cmake_minimum_required(VERSION 3.16)

project(encryption)

add_library(encryption
    src/encryption/encryption.cpp
    include/encryption/encryption.h
)

target_include_directories(encryption PRIVATE include/encryption PUBLIC include)

target_link_libraries(encryption PUBLIC cryptopp::cryptopp)
