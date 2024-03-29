cmake_minimum_required(VERSION 3.26)

# The main solution containing server and client projects
project(arcane)

if (VMP_DEBUG)
    add_compile_definitions(VMP_DEBUG)
    message("Configuring in debug mode")
else ()
    message("Configuring in release mode")
endif ()

set(CMAKE_GENERATOR_PLATFORM "Win32" CACHE STRING "" FORCE)

add_compile_definitions(ARCANE_SERVER_IP="")
add_compile_definitions(ARCANE_SERVER_KEY="")
add_compile_definitions(ARCANE_SERVER_PORT=1111)

add_compile_definitions(USE_PROTECTED_STRINGS=1)

# To compile a static MSVC runtime environment
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# To undefine max and min in order to use std::min and std::max
add_compile_definitions(NOMINMAX)

# To enable multiprocessor compilation
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

# Here we add subdirectories, thus calling their CMakeLists.txt
add_subdirectory(shared)
add_subdirectory(third-party)
add_subdirectory(client)
add_subdirectory(app)
add_subdirectory(updater)
