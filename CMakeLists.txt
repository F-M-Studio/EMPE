cmake_minimum_required(VERSION 3.30)
project(lidar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        SerialPort
        Charts
        REQUIRED)

add_executable(lidar main.cpp
        mainwindow.h
        mainwindow.cpp
        portsettings.cpp
        portsettings.h
        graphwindow.cpp
        graphwindow.h
        graphwindow.ui
        resources.qrc
)

target_link_libraries(lidar
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::SerialPort
        Qt6::Charts
)

# Windows platform-specific settings
if(WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if(MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif()
    foreach(QT_LIB Core Gui Widgets SerialPort Charts)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach(QT_LIB)

    # Add Windows resource file for icon
    set(WINDOWS_ICON_RC ${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc)
    file(WRITE ${WINDOWS_ICON_RC} "IDI_ICON1 ICON \"${CMAKE_CURRENT_SOURCE_DIR}/icons/EMPE.ico\"")
    target_sources(lidar PRIVATE ${WINDOWS_ICON_RC})
endif()

# macOS platform-specific settings
if(APPLE)
    set_target_properties(lidar PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER com.FMSquaredStudio.EMPE
            MACOSX_BUNDLE_BUNDLE_NAME "EMPE"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
            MACOSX_BUNDLE_ICON_FILE "EMPE.icns"
    )

    # Copy the icns file to the bundle
    set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/icons/EMPE.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    target_sources(lidar PRIVATE ${APP_ICON_MACOSX})
endif()

# Linux platform-specific settings
if(UNIX AND NOT APPLE)
    # Install binary
    install(TARGETS lidar DESTINATION bin)

    # Install desktop file
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/linux/empe.desktop.in
            ${CMAKE_CURRENT_BINARY_DIR}/empe.desktop @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/empe.desktop
            DESTINATION share/applications)

    # Install icons
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/EMPE.png
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME empe.png)

endif()