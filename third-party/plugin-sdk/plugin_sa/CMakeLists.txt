file(GLOB_RECURSE PSDK_GTASA_SOURCES "*.cpp" "*.h")
get_target_property(PSDK_SHARED_SOURCES shared SOURCES)

option(PSDK_VER_10US  ON)
option(PSDK_VER_10EU  OFF)
option(PSDK_VER_11US  OFF)
option(PSDK_VER_11EU  OFF)
option(PSDK_VER_SR2   OFF)
option(PSDK_VER_SR2LV OFF)

add_library(gtasa ${PSDK_GTASA_SOURCES} ${PSDK_SHARED_SOURCES})
add_library(pluginsdk::gtasa ALIAS gtasa)
add_dependencies(gtasa shared)

target_link_libraries(gtasa PUBLIC shared)
target_include_directories(gtasa
  PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/plugin_sa/;${PROJECT_SOURCE_DIR}/plugin_sa/game_sa/>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/plugin_sa/;${CMAKE_INSTALL_INCLUDEDIR}/plugin_sa/game_sa/>")
target_compile_definitions(gtasa
  PUBLIC
    GTASA
    GTAGAME_NAME="San Andreas"
    GTAGAME_ABBR="SA"
    GTAGAME_ABBRLOW="sa"
    GTAGAME_PROTAGONISTNAME="CJ"
    GTAGAME_CITYNAME="San Andreas"

    $<$<BOOL:${PSDK_VER_10US}>:PLUGIN_SGV_10US>
    $<$<BOOL:${PSDK_VER_10EU}>:PLUGIN_SGV_10EU>
    $<$<BOOL:${PSDK_VER_11US}>:PLUGIN_SGV_11US>
    $<$<BOOL:${PSDK_VER_11EU}>:PLUGIN_SGV_11EU>
    $<$<BOOL:${PSDK_VER_SR2}>:PLUGIN_SGV_SR2>
    $<$<BOOL:${PSDK_VER_SR2LV}>:PLUGIN_SGV_SR2LV>)
set_debug_postfix(gtasa)

if (PSDK_INSTALL)
  install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/plugin_sa/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/plugin_sa/
    FILES_MATCHING PATTERN "*.h" PATTERN "*.def")

  install(
    TARGETS gtasa
    EXPORT plugin-sdk-targets

    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  )
endif()