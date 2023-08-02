cmake_minimum_required(VERSION 3.16)

project(psdk_utils)

set(CMAKE_CXX_STANDARD 11)

add_library(${PROJECT_NAME} "include/psdk_utils/psdk_utils.h" "src/psdk_utils/psdk_utils.cpp" "include/psdk_utils/camera.h" "src/psdk_utils/camera.cpp" "include/psdk_utils/weapon.h" "src/psdk_utils/weapon.cpp" "include/psdk_utils/cmath.h" "src/psdk_utils/cmath.cpp" "include/psdk_utils/world.h" "include/psdk_utils/key.h" "src/psdk_utils/key.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE include/${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} pluginsdk::gtasa)