cmake_minimum_required(VERSION 3.20)

project(server VERSION 1.0 LANGUAGES CXX)

add_compile_options("-g")
add_compile_options("-fsanitize=thread")
add_link_options("-fsanitize=thread")

find_package(glog)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)
