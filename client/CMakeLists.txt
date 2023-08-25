cmake_minimum_required(VERSION 3.16)

project(client)

include(rc/CMakeRC.cmake)

set(CMAKE_CXX_STANDARD 20)

add_library(client SHARED
        src/main.cpp
        src/main.h
        src/plugin.cpp
        src/plugin.h
        src/socket.cpp
        src/socket.h
        src/query.cpp
        src/query.h
        src/client.cpp
        src/client.h
        src/unload.cpp
        src/unload.h
        src/game_logic_signals.hpp
        src/injection_in_game_logic.cpp
        src/injection_in_game_logic.h
        src/patches/camera_reset.cpp
        src/patches/camera_reset.h
        src/patches/fast_run.cpp
        src/patches/fast_run.h
        src/backends/imgui_impl_dx9.cpp
        src/backends/imgui_impl_dx9.h
        src/backends/imgui_impl_win32.cpp
        src/backends/imgui_impl_win32.h
        src/shooting/enemy_finder.cpp
        src/shooting/enemy_finder.h
        src/shooting/vector_aimbot.cpp
        src/shooting/vector_aimbot.h
        src/shooting/silent_aimbot.cpp
        src/shooting/silent_aimbot.h
        src/shooting/auto_cbug.cpp
        src/shooting/auto_cbug.h
        src/shooting/auto_shot.cpp
        src/shooting/auto_shot.h
        src/visuals/visuals.cpp
        src/visuals/visuals.h
        src/actor/actor.cpp
        src/actor/actor.h
        src/actor/air_walking.cpp
        src/actor/air_walking.h
        src/actor/anti_stun.cpp
        src/actor/anti_stun.h
        src/actor/fast_run.cpp
        src/actor/fast_run.h
        src/actor/spread_control.cpp
        src/actor/spread_control.h
        src/vehicle/vehicle.cpp
        src/vehicle/vehicle.h
        src/vehicle/infinite_nitro.cpp
        src/vehicle/infinite_nitro.h
        src/actor/infinite_clip.cpp
        src/actor/infinite_clip.h
        src/actor/disable_camera_reset.cpp
        src/actor/disable_camera_reset.h
        src/actor/infinite_ammo.cpp
        src/actor/infinite_ammo.h
        src/actor/infinite_ammo.h
        src/actor/infinite_run.cpp
        src/actor/infinite_run.h
        src/vehicle/drive_on_water.cpp
        src/vehicle/drive_on_water.h
        src/patches/drive_on_water.cpp
        src/patches/drive_on_water.h
        src/vehicle/fast_exit.cpp
        src/vehicle/fast_exit.h
        src/actor/auto_reload.cpp
        src/actor/auto_reload.h
)

find_package(nlohmann_json REQUIRED)
find_package(imgui REQUIRED)

cmrc_add_resource_library(
        fonts
        rc/fonts/jetbrains_mono_bold.ttf
        rc/fonts/geforce_bold.ttf
)

target_link_libraries(client PRIVATE
        ws2_32
        wsock32
        imgui::imgui
        nlohmann_json::nlohmann_json
        pluginsdk::gtasa
        sampapi
        kthook
        data_representation
        psdk_utils
        samp_utils
        winapi_utils
        utils
        encryption
        arcane_packets
        vmprotect-sdk
        fonts
        protected_string
        infoware
)

if (VMP_DEBUG)
    set_target_properties(client
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${ASI_PLUGIN_OUTPUT_DIR}
            LIBRARY_OUTPUT_DIRECTORY ${ASI_PLUGIN_OUTPUT_DIR}
            RUNTIME_OUTPUT_DIRECTORY ${ASI_PLUGIN_OUTPUT_DIR}
            SUFFIX ".asi"
    )
endif ()