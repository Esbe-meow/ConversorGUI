cmake_minimum_required(VERSION 3.19)
project(ConversorGUI LANGUAGES CXX)

include(GNUInstallDirs)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(ConversorGUI
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

qt_add_resources(ConversorGUI "resources"
    PREFIX "/"
    FILES
        resources/icon.png
)

target_link_libraries(ConversorGUI
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS ConversorGUI
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
