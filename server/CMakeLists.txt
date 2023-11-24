cmake_minimum_required(VERSION 3.20)

project(server VERSION 1.0 LANGUAGES CXX)

find_package(glog)

add_subdirectory(src)
add_subdirectory(tests)
