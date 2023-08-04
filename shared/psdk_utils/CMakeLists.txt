cmake_minimum_required(VERSION 3.16)

project(psdk_utils)

set(CMAKE_CXX_STANDARD 11)

add_library(psdk_utils
    include/psdk_utils/camera.h
    include/psdk_utils/cmath.h
    include/psdk_utils/key.h
    include/psdk_utils/local_vector.h
    include/psdk_utils/polar_vector.h
    include/psdk_utils/psdk_utils.h
    include/psdk_utils/weapon.h
    include/psdk_utils/world.h
    src/psdk_utils/camera.cpp
    src/psdk_utils/cmath.cpp
    src/psdk_utils/key.cpp
    src/psdk_utils/local_vector.cpp
    src/psdk_utils/polar_vector.cpp
    src/psdk_utils/psdk_utils.cpp
    src/psdk_utils/weapon.cpp
    src/psdk_utils/world.cpp)

target_include_directories(psdk_utils PRIVATE include/psdk_utils PUBLIC include)

target_link_libraries(psdk_utils pluginsdk::gtasa)
