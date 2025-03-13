cmake_minimum_required(VERSION 3.30)
project(lidar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Hardcoded path for qt 6.7.3 on windows
# set(CMAKE_PREFIX_PATH "C:/Qt/6.7.3/mingw_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        SerialPort
        Charts
        REQUIRED)

add_executable(lidar WIN32 main.cpp
        mainwindow.h
        mainwindow.cpp
        portsettings.cpp
        portsettings.h
        graphwindow.cpp
        graphwindow.h
        graphwindow.ui
)

target_link_libraries(lidar
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::SerialPort
        Qt6::Charts
)

#hard coded paths so the dlls actually copy 
include(GNUInstallDirs)
if(WIN32)
    set(DEBUG_SUFFIX "")
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()

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
    set(MINGW_BIN_PATH "C:/Qt/Tools/mingw1120_64/bin")
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
