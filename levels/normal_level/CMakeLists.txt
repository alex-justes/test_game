cmake_minimum_required(VERSION 3.10)
project(normal_level)

add_library(
        normal_level
        SHARED
        ""
)

target_compile_features(
    normal_level
    PUBLIC
        cxx_std_17
)

target_compile_definitions(
        normal_level
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(
        normal_level
        PRIVATE
        $<BUILD_INTERFACE:${normal_level_SOURCE_DIR}/include>
)

target_link_libraries(
        normal_level
        PRIVATE
        core::engine
        extensions::support
)

target_sources(
        normal_level
        PRIVATE
        include/Context.h
        src/Context.cpp
        src/entry_point.cpp
)

