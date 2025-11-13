cmake_minimum_required(VERSION 3.5)

project(qtcefwindowless VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

set(PROJECT_SOURCES
        main.cpp
        browserwindow.cpp
        browserwindow.h
        qtcefapp.h
        qtcefapp.cpp
        qtcefclient.h
        qtcefclient.cpp
        cefwidget.h
        cefwidget.cpp
        addressbar.h
        addressbar.cpp
        titlebar.h
        titlebar.cpp
)
add_definitions(-DNOMINMAX)
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
target_include_directories(qtcefwindowless PRIVATE ${PROJECT_SOURCE_DIR}/cef/include)
target_link_directories(qtcefwindowless PRIVATE ${PROJECT_SOURCE_DIR}/cef/libs)

# qt libs
target_link_libraries(qtcefwindowless PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGL Qt${QT_VERSION_MAJOR}::OpenGLWidgets libcef_dll_wrapper libcef)

# cef
target_link_libraries(qtcefwindowless PRIVATE $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/cef/libs/Debug/libcef_dll_wrapper.lib> $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/cef/libs/Release/libcef_dll_wrapper.lib>)
target_link_libraries(qtcefwindowless PRIVATE $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/cef/libs/Debug/libcef.lib> $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/cef/libs/Release/libcef.lib>)

add_custom_command(TARGET qtcefwindowless POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/cef/bin/"
        "$<TARGET_FILE_DIR:qtcefwindowless>/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/cef/resources/"
        "$<TARGET_FILE_DIR:qtcefwindowless>/"
    COMMENT "Copying assets files to output directory"
)

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
