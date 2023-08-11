# Here we connect libraries to the solution

cmake_minimum_required(VERSION 3.16)

# 1. After adding a subdirectory to the solution there is calling its CMakeLists.txt which configures the library
# 2. After that the subdirectory becomes visible to all projects
# 3. In order to connect the library to the project, we have to call target_link_libraries(${PROJECT_NAME} PRIVATE <name>)

option(INFOWARE_USE_D3D "" ON)

add_subdirectory(cryptopp-cmake)
add_subdirectory(plugin-sdk)
add_subdirectory(kthook)
add_subdirectory(sampapi)
add_subdirectory(encryption)
add_subdirectory(plog)
add_subdirectory(vmprotect-sdk)
add_subdirectory(infoware)