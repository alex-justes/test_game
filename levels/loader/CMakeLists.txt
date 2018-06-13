cmake_minimum_required(VERSION 3.10)
project(loader)

add_library(
        loader
        SHARED
        ""
)

target_compile_features(
    loader
    PUBLIC
        cxx_std_17
)

target_compile_definitions(
        loader
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(
        loader
        PRIVATE
        $<BUILD_INTERFACE:${loader_SOURCE_DIR}/include>
)

target_link_libraries(
        loader
        PRIVATE
        core::engine
)

target_sources(
        loader
        PRIVATE
        include/Context.h
        src/Context.cpp
        src/entry_point.cpp
)

