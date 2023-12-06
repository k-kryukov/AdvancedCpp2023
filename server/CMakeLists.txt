cmake_minimum_required(VERSION 3.20)

project(server VERSION 1.0 LANGUAGES CXX)

add_compile_options("-g")

find_package(glog)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)
