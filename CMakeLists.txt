cmake_minimum_required(VERSION 3.26)

# The main solution containing server and client projects
project(arcane)

add_compile_options(
    $<$<CONFIG:>:/MT>
    $<$<CONFIG:Debug>:/MTd>
    $<$<CONFIG:Release>:/MT>
)

if (VMP_DEBUG)
    add_compile_definitions(VMP_DEBUG)
    message("Configuring in debug mode")
else ()
    message("Configuring in release mode")
endif ()

add_compile_definitions(ARCANE_SERVER_IP="31.129.32.99")
add_compile_definitions(ARCANE_SERVER_KEY="Ebm09q37v3EMX8Z3")
add_compile_definitions(ARCANE_SERVER_PORT=4289)

add_compile_definitions(USE_PROTECTED_STRINGS=1)

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
