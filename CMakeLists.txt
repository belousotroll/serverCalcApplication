cmake_minimum_required(VERSION 3.16)

project(serverCalcApplication CXX)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CONFIGURATION_TYPES Debug Release Profile)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

### Find out compiler's flags

set(CMAKE_CXX_FLAGS "-g3 -Wall -Wextra -Wunreachable-code -pedantic")

set(CMAKE_C_FLAGS_PROFILE "-pg -O0 -fno-inline")
set(CMAKE_CXX_FLAGS_PROFILE "-pg -O0 -fno-inline")

set(CMAKE_C_FLAGS_DEBUG "-O0 -fno-inline")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline")

set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

find_package(Boost 1.69.0 COMPONENTS context thread coroutine REQUIRED)

add_subdirectory(external)

# PostgreSQL library

find_package(PostgreSQL REQUIRED)
find_library(PQXX_LIB pqxx)

file(GLOB_RECURSE ${PROJECT_NAME}_CPP_SOURCES "src/*.cpp")

set(${PROJECT_NAME}_INCLUDES
        ${PostgreSQL_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIR})

set(${PROJECT_NAME}_LIBS
        tinyexpr
        yandex::ozo
        pthread
        pqxx
        pq
        Boost::context
        Boost::coroutine)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_CPP_SOURCES} main.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        ${${PROJECT_NAME}_INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_LIBS})

