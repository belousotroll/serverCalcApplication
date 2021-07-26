cmake_minimum_required(VERSION 3.19)

project(server)

set(CMAKE_CXX_STANDARD 20)

if (NOT BOOST_FOUND)
    set(BOOST_ROOT "/opt/boost")
endif ()

find_package(Boost 1.69.0 COMPONENTS system REQUIRED)

set(SERVER_SOURCES
        Server.h Server.cpp
        Connection.h Connection.cpp
        ConnectionPool.h ConnectionPool.cpp)

add_executable(${PROJECT_NAME} main.cpp ${SERVER_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})

# Boost::asio работает с потоками, поэтому необходимо подключить библиотеку pthread.
# На Windows нужно подключить библиотеки для работы с winsocks (ws2_32.dll ...)
target_link_libraries(${PROJECT_NAME} PUBLIC pthread Boost::system)

