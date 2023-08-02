cmake_minimum_required(VERSION 3.16)

project(winapi_utils)

set(CMAKE_CXX_STANDARD 17)

# VMProtect SDK
set(VMPROTECT_SDK_INCLUDE_PATH "C:/Program Files/VMProtect Ultimate/Include/C")
set(VMPROTECT_SDK_LIBRARY_PATH "C:/Program Files/VMProtect Ultimate/Lib/Windows/VMProtectSDK32.lib")

add_library(${PROJECT_NAME}
    include/${PROJECT_NAME}/${PROJECT_NAME}.h
    src/${PROJECT_NAME}/${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    include/${PROJECT_NAME}
    ${VMPROTECT_SDK_INCLUDE_PATH}
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    data_representation
    ${VMPROTECT_SDK_LIBRARY_PATH}
)