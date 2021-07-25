cmake_minimum_required(VERSION 3.19)

project(server)

set(CMAKE_CXX_STANDARD 20)

if (NOT BOOST_FOUND)
    set(BOOST_ROOT "/opt/boost")
endif ()

find_package(Boost 1.69.0 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
