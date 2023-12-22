
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(glog REQUIRED)
find_package(Boost REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

include_directories(../src/include)

enable_testing(true)

add_executable(client-tests tests.cpp)
target_link_libraries(client-tests PRIVATE glog::glog Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt::Test)
