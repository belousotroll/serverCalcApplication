cmake_minimum_required(VERSION 3.19)

project(server)

SET(CMAKE_CXX_COMPILER g++)

if (NOT BOOST_FOUND)
    set(BOOST_ROOT "/opt/boost")
endif()

find_package(Boost 1.69.0 COMPONENTS context thread coroutine REQUIRED)
find_package(PostgreSQL 13.3 REQUIRED)

set(DATABASE_SOURCES
        database/PostgreSQLDatabase.cpp database/PostgreSQLDatabase.h)

set(MODELS_SOURCES
        models/Structures.h)

set(SERVER_SOURCES
        Server.cpp Server.h
        Connection.cpp Connection.h
        ConnectionPool.cpp ConnectionPool.h)

set(EXTERNAL_LIBRARIES_DIR
        ../external)

set(TINYEXPR_SOURCES
        ${EXTERNAL_LIBRARIES_DIR}/tinyexpr/tinyexpr.c
        ${EXTERNAL_LIBRARIES_DIR}/tinyexpr/tinyexpr.h)

message(STATUS "${PostgreSQL_INCLUDE_DIRS}")

add_executable(${PROJECT_NAME} main.cpp
        ${SERVER_SOURCES}
        ${DATABASE_SOURCES}
        ${MODELS_SOURCES}
        ${TINYEXPR_SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC database models)
target_include_directories(${PROJECT_NAME} PUBLIC
        ${PostgreSQL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${EXTERNAL_LIBRARIES_DIR}/ozo/include
        ${EXTERNAL_LIBRARIES_DIR}/ozo/contrib/resource_pool/include
        ${EXTERNAL_LIBRARIES_DIR}/tinyexpr)

target_link_directories(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC pthread pqxx pq Boost::context Boost::coroutine)

