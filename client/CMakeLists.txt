cmake_minimum_required(VERSION 3.5)

project(client)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(GTest REQUIRED)
find_package(glog REQUIRED)
find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(client main.cpp)
target_link_libraries(client Qt5::Core Qt5::Widgets glog::glog Boost::headers nlohmann_json::nlohmann_json)