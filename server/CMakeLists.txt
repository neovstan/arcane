cmake_minimum_required(VERSION 3.16)

project(server)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-libmariadb CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/socket/socket.h
	src/socket/socket.cpp
	src/database/database.h
	src/database/database.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	wsock32
	ws2_32
	unofficial::libmariadb
	mariadbclientpp
	nlohmann_json::nlohmann_json
	ktsignal
	plog
	encryption
	configuration
	data_representation
)