set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(service Service.cpp)
target_link_libraries(service glog::glog)
target_include_directories(service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
