cmake_minimum_required(VERSION 3.5)

project(client VERSION 1.0 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)
find_package(glog)

add_subdirectory(src)
add_subdirectory(tests)