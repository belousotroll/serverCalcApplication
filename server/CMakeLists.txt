cmake_minimum_required(VERSION 3.19)

project(server)

SET(CMAKE_CXX_COMPILER g++)

if (NOT BOOST_FOUND)
    set(BOOST_ROOT "/opt/boost")
endif()

find_package(Boost 1.69.0 COMPONENTS context thread coroutine REQUIRED)
find_package(PostgreSQL 13.3 REQUIRED)

set(DATABASE_SOURCES
        database/IDatabaseAccessor.h
        database/PostgreSQLDatabase.cpp database/PostgreSQLDatabase.h
        database/Structures.h)

set(SERVER_SOURCES
        Server.cpp Server.h
        Connection.cpp Connection.h
        ConnectionPool.cpp ConnectionPool.h)

set(EXTERNAL_LIBRARIES_DIR
        ../external)

set(TINYEXPR
        ${EXTERNAL_LIBRARIES_DIR}/tinyexpr/tinyexpr.c
        ${EXTERNAL_LIBRARIES_DIR}/tinyexpr/tinyexpr.h)


add_executable(${PROJECT_NAME} main.cpp
        ${SERVER_SOURCES}
        ${DATABASE_SOURCES}
        ${TINYEXPR}
        )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC database)
target_include_directories(${PROJECT_NAME} PUBLIC
        ${PostgreSQL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${EXTERNAL_LIBRARIES_DIR}/ozo/include
        ${EXTERNAL_LIBRARIES_DIR}/ozo/contrib/resource_pool/include
        ${EXTERNAL_LIBRARIES_DIR}/tinyexpr)

target_link_directories(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})

# Boost::asio работает с потоками, поэтому необходимо подключить библиотеку pthread.
# На Windows нужно подключить библиотеки для работы с winsocks (ws2_32.dll ...)
target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
        pqxx pq
        Boost::context
        Boost::coroutine)

