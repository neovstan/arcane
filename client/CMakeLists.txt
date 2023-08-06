cmake_minimum_required(VERSION 3.16)

project(client)

set(CMAKE_CXX_STANDARD 20)

add_library(client SHARED
	src/main.cpp
	src/main.h
	src/plugin.cpp
	src/plugin.h
	src/socket.cpp
	src/socket.h
	src/game_logic_signals.hpp 
	src/injection_in_game_logic.cpp
	src/injection_in_game_logic.h
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
)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(CMakeRC CONFIG REQUIRED)

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
	configuration
	vmprotect-sdk
	fonts
)

if (VMP_DEBUG)
	set_target_properties(client
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "C:/Users/me/Games/GTA San Andreas"
		LIBRARY_OUTPUT_DIRECTORY "C:/Users/me/Games/GTA San Andreas"
		RUNTIME_OUTPUT_DIRECTORY "C:/Users/me/Games/GTA San Andreas"
		SUFFIX ".asi"
	)
endif()