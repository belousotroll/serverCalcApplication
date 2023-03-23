add_subdirectory(ozo)
add_subdirectory(fmt)

# create library from sources

project(tinyexpr)
add_library(tinyexpr STATIC tinyexpr/tinyexpr.c)
target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tinyexpr>)
