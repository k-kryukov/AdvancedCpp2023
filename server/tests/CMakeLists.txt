
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glog)
find_package(GTest REQUIRED)
find_package(glog REQUIRED)
find_package(Boost REQUIRED)

include(GoogleTest)

add_executable(server-tests tests.cpp)
target_link_libraries(server-tests glog::glog gtest)
gtest_discover_tests(server-tests)