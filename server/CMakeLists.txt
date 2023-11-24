cmake_minimum_required(VERSION 3.20)

project(server VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glog)

add_executable(server main.cpp)
target_link_libraries(server glog::glog)