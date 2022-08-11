cmake_minimum_required(VERSION 3.5)

project(qtcefwindowless VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
        qtcefapp.h
        qtcefapp.cpp
        qtcefclient.h
        qtcefclient.cpp
        mainwindow.ui
)
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qtcefwindowless
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qtcefwindowless APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(qtcefwindowless SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qtcefwindowless
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(qtcefwindowless PRIVATE ${PROJECT_SOURCE_DIR}/cef)
target_link_directories(qtcefwindowless PRIVATE ${PROJECT_SOURCE_DIR}/cef/libs)
target_link_libraries(qtcefwindowless PRIVATE Qt${QT_VERSION_MAJOR}::Widgets libcef_dll_wrapper libcef)

#add_library(libcef SHARED IMPORTED GLOBAL)
#set_target_properties(libcef PROPERTIES
#    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/cef/include"            #PUBLIC头文件目录
#    IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/cef/libs/libcef.lib" #Windows平台上dll库的.lib库所在位置
#    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/cef/libs/libcef.dll"  #dll库的.dll所在位置，或者.so库的位置，或者静态库的位置
#)

set_target_properties(qtcefwindowless PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS qtcefwindowless
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qtcefwindowless)
endif()
