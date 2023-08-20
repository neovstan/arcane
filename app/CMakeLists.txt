cmake_minimum_required(VERSION 3.26)
project(app)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH QT_PATH)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Svg
        REQUIRED
)

find_package(nlohmann_json REQUIRED)

add_executable(app WIN32
        rc/app.rc
        rc/app.qrc
        src/main.cpp
        src/main_window.ui
        src/main_window.cpp
        src/main_window.h
        src/authorization.ui
        src/authorization.cpp
        src/authorization.h
        src/home.ui
        src/home.cpp
        src/home.h
        src/notification.cpp
        src/notification.h
        src/client.cpp
        src/client.h
        src/query.cpp
        src/query.h
        src/manual_map.cpp
        src/manual_map.h
        src/inline_syscall.cpp
        src/inline_syscall.h
        src/packets/initialization.hpp
        src/packets/load.hpp
        src/packets/update.hpp
)

target_link_libraries(app
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::Svg
        nlohmann_json::nlohmann_json
        utils
        winapi_utils
        protected_string
        vmprotect-sdk
        encryption
)

set_target_properties(app PROPERTIES CXX_STANDARD 17)

if (WIN32)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/imageformats/qsvg${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imageformats/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/imageformats/qsvg${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imageformats/")
    endif ()
    foreach (QT_LIB Core Gui Widgets Network Svg)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
