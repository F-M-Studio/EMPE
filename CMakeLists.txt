# Nazwa Projektu: EMPE
# Plik: CMakeLists.txt
#
# Krótki opis pliku: Główny plik konfiguracyjny CMake zawierający instrukcje budowania projektu i zarządzania zależnościami.
#
# Autorzy:
# Mateusz Korniak <mkorniak04@gmail.com>
# Mateusz Machowski <machowskimateusz51@gmail.com>
# Filip Leśnik <filip.lesnik170@gmail.com>
#
# Data Utworzenia: 4 Marca 2025
# Ostatnia Modyfikacja: 27 Czerwca 2025
#
# Ten program jest wolnym oprogramowaniem; możesz go rozprowadzać i/lub
# modyfikować na warunkach Powszechnej Licencji Publicznej GNU,
# opublikowanej przez Free Software Foundation, w wersji 3 tej Licencji
# lub (według twojego wyboru) dowolnej późniejszej wersji.
#
# Ten program jest rozpowszechniany w nadziei, że będzie użyteczny, ale
# BEZ ŻADNEJ GWARANCJI; nawet bez domyślnej gwarancji PRZYDATNOŚCI
# HANDLOWEJ lub PRZYDATNOŚCI DO OKREŚLONEGO CELU. Zobacz Powszechną
# Licencję Publiczną GNU, aby uzyskać więcej szczegółów.
#
# Powinieneś otrzymać kopię Powszechnej Licencji Publicznej GNU wraz z
# tym programem. Jeśli nie, zobacz <http://www.gnu.org/licenses/>.

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

# Konfiguracja tłumaczeń
set(TS_FILES
        ${CMAKE_SOURCE_DIR}/translations/lidar_en.ts
        ${CMAKE_SOURCE_DIR}/translations/lidar_pl.ts
)

qt_add_translations(lidar
        TS_FILES ${TS_FILES}
        QM_FILES_OUTPUT_VARIABLE QM_FILES
)

if(DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "Running in GitHub Actions CI environment")
    set(CI_MODE TRUE)
endif()

# Definicja celu wykonywalnego (zależna od platformy)
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
elseif(UNIX OR APPLE)
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

# Linkowanie bibliotek
target_link_libraries(lidar
        PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::SerialPort
        Qt6::Charts
)

# --- Polecenia POST_BUILD (po definicji celu) ---

# Kopiowanie skompilowanych plików tłumaczeń (.qm)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/translations"
        COMMENT "Creating translations directory..."
)
foreach(QM_FILE ${QM_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${QM_FILE}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/translations/"
            COMMENT "Copying ${QM_FILE}..."
    )
endforeach()

# Ustawienia wdrożenia (deployment)
include(GNUInstallDirs)

if(WIN32)
    set(DEBUG_SUFFIX "")
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(WINDOWS_ICON_RC ${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc)
    file(WRITE ${WINDOWS_ICON_RC} "IDI_ICON1 ICON \"${CMAKE_CURRENT_SOURCE_DIR}/icons/EMPE.ico\"")
    target_sources(lidar PRIVATE ${WINDOWS_ICON_RC})

    if(CI_MODE)
        message(STATUS "Configuring for GitHub Actions CI")
        set(QT_PATH $ENV{QT_PATH})
        set(MINGW_BIN_PATH "$ENV{MINGW_HOME}/bin")

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND "${QT_PATH}/bin/windeployqt.exe"
                --verbose 1
                --compiler-runtime
                --dir \"$<TARGET_FILE_DIR:${PROJECT_NAME}>/../\"
                \"$<TARGET_FILE:${PROJECT_NAME}>\"
                COMMENT "CI: Deploying Qt dependencies..."
        )
        foreach(MINGW_DLL libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_BIN_PATH}/${MINGW_DLL}"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                    COMMENT "CI: Copying ${MINGW_DLL}..."
            )
        endforeach()
    else()
        # Wdrożenie lokalne na Windows
        set(QT_PATH "F:/Qt/6.7.3/mingw_64") # Upewnij się, że ścieżka jest poprawna
        if(EXISTS "${QT_PATH}/bin/windeployqt.exe")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND "${QT_PATH}/bin/windeployqt.exe"
                    --verbose 0
                    --no-compiler-runtime
                    --dir \"$<TARGET_FILE_DIR:${PROJECT_NAME}>\"
                    \"$<TARGET_FILE:${PROJECT_NAME}>\"
                    COMMENT "Deploying Qt dependencies..."
            )
        endif()

        set(MINGW_BIN_PATH "F:/Qt/Tools/mingw1120_64/bin") # Upewnij się, że ścieżka jest poprawna
        foreach(MINGW_DLL libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_BIN_PATH}/${MINGW_DLL}"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                    COMMENT "Copying ${MINGW_DLL}..."
            )
        endforeach()
    endif()
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

    set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/icons/EMPE.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    target_sources(lidar PRIVATE ${APP_ICON_MACOSX})
endif()

# Linux platform-specific settings
if(UNIX AND NOT APPLE)
    install(TARGETS lidar DESTINATION bin)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/empe.desktop
            DESTINATION share/applications)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/icons/EMPE.png
            DESTINATION share/icons/hicolor/256x256/apps
            RENAME empe.png)
endif()
