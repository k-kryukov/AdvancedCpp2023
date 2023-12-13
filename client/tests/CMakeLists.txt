
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(GTest REQUIRED)
find_package(glog REQUIRED)
find_package(Boost REQUIRED)

include(GoogleTest)

include_directories(../src/include)

add_executable(client-tests tests.cpp)
target_link_libraries(client-tests glog::glog gtest Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

add_executable(almost_echo_server almost_echo_server.cpp)
target_link_libraries(almost_echo_server glog::glog)

gtest_discover_tests(client-tests)