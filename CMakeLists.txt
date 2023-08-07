cmake_minimum_required(VERSION 3.16)

# The main solution containing server and client projects
project(arcane)

option(VMP_DEBUG "" ON)

if (VMP_DEBUG)
    add_compile_definitions(VMP_DEBUG)
    add_compile_definitions(ARCANE_SERVER_IP="localhost")
else ()
    add_compile_definitions(ARCANE_SERVER_IP="45.136.204.97")
endif ()

add_compile_definitions(ARCANE_SERVER_KEY="KsbnIQ8th5n9ua9O")
add_compile_definitions(ARCANE_SERVER_PORT="86724")

# To compile a static MSVC runtime environment
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# To undefine max and min in order to use std::min and std::max
add_compile_definitions(NOMINMAX)

# To enable multiprocessor compilation
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

# Here we add subdirectories, thus calling their CMakeLists.txt
add_subdirectory(shared)
add_subdirectory(third-party)
add_subdirectory(client)

unset(VMP_DEBUG CACHE)