cmake_minimum_required(VERSION 3.19)

project(server)

SET(CMAKE_CXX_COMPILER g++)

if (NOT BOOST_FOUND)
    set(BOOST_ROOT "/opt/boost")
endif()

find_package(Boost 1.69.0 COMPONENTS system REQUIRED)
find_package(PostgreSQL 13.3 REQUIRED)

set(SERVER_SOURCES
        Server.cpp Server.h
        Connection.cpp Connection.h
        ConnectionPool.cpp ConnectionPool.h
        RequestHandler.cpp RequestHandler.h)

set(EXTERNAL_LIBRARIES_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../external)

set(SERVER_TEMPLATE_HEADERS
        Request.h)

add_executable(${PROJECT_NAME} main.cpp
        ${SERVER_SOURCES}
        ${SERVER_TEMPLATE_HEADERS})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PostgreSQL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR}
        ${EXTERNAL_LIBRARIES_DIR}/ozo/include)

# Boost::asio работает с потоками, поэтому необходимо подключить библиотеку pthread.
# На Windows нужно подключить библиотеки для работы с winsocks (ws2_32.dll ...)
target_link_libraries(${PROJECT_NAME} PUBLIC pthread)

