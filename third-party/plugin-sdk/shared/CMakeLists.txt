file(GLOB_RECURSE PSDK_SHARED_SOURCES "*.cpp" "*.h")

option(PSDK_DXSDK "DirectX 9 SDK" OFF)

add_library(shared INTERFACE ${PSDK_SHARED_SOURCES})
add_library(pluginsdk::shared ALIAS shared)

target_include_directories(shared
  INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/shared/;${PROJECT_SOURCE_DIR}/shared/game/>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/shared/;${CMAKE_INSTALL_INCLUDEDIR}/shared/game/>")
target_compile_features(shared INTERFACE cxx_std_17)
target_compile_definitions(shared
  INTERFACE
    _CRT_SECURE_NO_WARNINGS
    _CRT_NON_CONFORMING_SWPRINTFS)

if (PSDK_DXSDK)
  set(PSDK_DXSDK_DIR $ENV{DIRECTX9_SDK_DIR} CACHE STRING "")
  if (NOT DEFINED PSDK_DXSDK_DIR)
    message(FATAL_ERROR "Not found DirectX9 SDK directory.")
  endif()

  target_compile_definitions(shared INTERFACE _DX9_SDK_INSTALLED)
  target_include_directories(shared INTERFACE ${PSDK_DXSDK_DIR}/Include/)
  target_link_directories(shared INTERFACE ${PSDK_DXSDK_DIR}/Lib/x86/)
  target_link_libraries(shared INTERFACE d3d9.lib d3dx9.lib)
endif()

if (PSDK_INSTALL)
  install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/shared/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/shared/
    FILES_MATCHING PATTERN "*.h")

  install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/injector/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/injector/
    FILES_MATCHING PATTERN "*.hpp")

  install(TARGETS shared EXPORT plugin-sdk-targets)
endif()