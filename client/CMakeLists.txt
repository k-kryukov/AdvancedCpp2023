cmake_minimum_required(VERSION 3.5)

project(client)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

add_executable(client main.cpp)
target_link_libraries(client Qt5::Core Qt5::Widgets)