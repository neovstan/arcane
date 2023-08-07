cmake_minimum_required(VERSION 3.16)

project(loader)

set(CMAKE_CXX_STANDARD 20)

# VMProtect SDK
set(VMPROTECT_SDK_INCLUDE_PATH "C:/Program Files/VMProtect Ultimate/Include/C")
set(VMPROTECT_SDK_LIBRARY_PATH "C:/Program Files/VMProtect Ultimate/Lib/Windows/VMProtectSDK32.lib")

find_package(glfw3 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(imgui REQUIRED)

add_executable(${PROJECT_NAME} WIN32
	src/main.cpp
	src/backends/imgui_impl_glfw.cpp
	src/backends/imgui_impl_glfw.h
	src/backends/imgui_impl_opengl2.cpp
	src/backends/imgui_impl_opengl2.h
	src/socket/socket.cpp
	src/socket/socket.h
	src/manualmap/manualmap.cpp
	src/manualmap/manualmap.h
	src/manualmap/winapi.cpp
	src/manualmap/winapi.h
	src/main_window.cpp
	src/main_window.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${VMPROTECT_SDK_INCLUDE_PATH}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	ws2_32
	psapi
	wsock32
	glfw
	opengl32
	nlohmann_json::nlohmann_json
	ktsignal
	imgui::imgui
	${VMPROTECT_SDK_LIBRARY_PATH}
	encryption
	configuration
	data_representation
	winapi_utils
)