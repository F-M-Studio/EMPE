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
        LinguistTools
        REQUIRED
)

if(DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "Running in GitHub Actions CI environment")
    set(CI_MODE TRUE)
endif()

if(WIN32)
    add_executable(lidar WIN32 main.cpp
            mainwindow.h
            mainwindow.cpp
            portsettings.cpp
            portsettings.h
            graphwindow.cpp
            graphwindow.h
            graphwindow.ui
            resources.qrc
            appmenu.h
            appmenu.cpp
            aboutusdialog.h
            aboutusdialog.cpp
    )
        if(CI_MODE)
                message(STATUS "Configuring for GitHub Actions CI")
                
                # Use environment variables from GitHub Actions
                set(QT_PATH $ENV{QT_PATH})
                set(MINGW_BIN_PATH "$ENV{MINGW_HOME}/bin")
                
                # Simplified deployment command
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND "${QT_PATH}/bin/windeployqt.exe"
                    --verbose 1
                    --compiler-runtime
                    --no-translations
                    --dir \"$<TARGET_FILE_DIR:${PROJECT_NAME}>/../\"
                    \"$<TARGET_FILE:${PROJECT_NAME}>\"
                    COMMENT "CI: Deploying Qt dependencies..."
                )
                
                # Copy MinGW runtime DLLs from CI environment
                foreach(MINGW_DLL
                    libgcc_s_seh-1.dll
                    libstdc++-6.dll
                    libwinpthread-1.dll)
                    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${MINGW_BIN_PATH}/${MINGW_DLL}"
                        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                        COMMENT "CI: Copying ${MINGW_DLL}..."
                    )
                endforeach()
                
            else()
    endif()
endif ()

if (UNIX OR APPLE)
    add_executable(lidar main.cpp
            mainwindow.h
            mainwindow.cpp
            portsettings.cpp
            portsettings.h
            graphwindow.cpp
            graphwindow.h
            graphwindow.ui
            resources.qrc
            appmenu.h
            appmenu.cpp
            aboutusdialog.h
            aboutusdialog.cpp
    )
endif ()

qt_add_translations(lidar
        TS_FILES
        translations/lidar_pl.ts
        translations/lidar_en.ts
        QM_FILES_OUTPUT_VARIABLE QM_FILES
)

target_link_libraries(lidar
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::SerialPort
        Qt6::Charts
)

# Add deployment
include(GNUInstallDirs)
if(WIN32)
    set(DEBUG_SUFFIX "")
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(WINDOWS_ICON_RC ${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc)
    file(WRITE ${WINDOWS_ICON_RC} "IDI_ICON1 ICON \"${CMAKE_CURRENT_SOURCE_DIR}/icons/EMPE.ico\"")
    target_sources(lidar PRIVATE ${WINDOWS_ICON_RC})
    # Deploy Qt runtime files
    if(TARGET Qt6::qmake)
        get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

        # Deploy Qt plugins
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND "${_qt_bin_dir}/windeployqt.exe"
                --verbose 0
                --no-compiler-runtime
                --no-translations
                --no-system-d3d-compiler
                \"$<TARGET_FILE:${PROJECT_NAME}>\"
                COMMENT "Deploying Qt dependencies..."
        )
    endif()

    # Deploy MinGW runtime DLLs
    set(MINGW_BIN_PATH "F:/Qt/Tools/mingw1120_64/bin")
    foreach(MINGW_DLL
            libgcc_s_seh-1.dll
            libstdc++-6.dll
            libwinpthread-1.dll)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_PATH}/${MINGW_DLL}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                COMMENT "Copying ${MINGW_DLL}..."
        )
    endforeach()
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

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/empe.desktop
            DESTINATION share/applications)

    # Install icons
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/EMPE.png
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME empe.png)

endif()
