cmake_minimum_required(VERSION 3.16)

project(plugin-sdk LANGUAGES C CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

function(set_debug_postfix target)
  set_target_properties(${target} PROPERTIES DEBUG_POSTFIX "_d")
endfunction()

add_subdirectory(shared)
add_subdirectory(plugin_sa)