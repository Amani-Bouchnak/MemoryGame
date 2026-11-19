cmake_minimum_required(VERSION 3.16)

project(MemoryGame VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MemoryGame
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        carte.h carte.cpp
        plateau.h plateau.cpp
        jeu.h jeu.cpp
        score.h score.cpp
    )

    # Intégration automatique de vos 10 images dans les ressources Qt
    qt_add_resources(MemoryGame "app_images"
        PREFIX "/"
        FILES
            images/1.png
            images/2.png
            images/3.png
            images/4.png
            images/5.png
            images/6.png
            images/7.png
            images/8.png
            images/9.png
            images/10.png
    )
else()
    if(ANDROID)
        add_library(MemoryGame SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(MemoryGame
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MemoryGame PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MemoryGame)
endif()
set_target_properties(MemoryGame PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MemoryGame
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MemoryGame)
endif()