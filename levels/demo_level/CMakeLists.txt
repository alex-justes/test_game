cmake_minimum_required(VERSION 3.10)
project(demo_level)

add_library(
        demo_level
        SHARED
        ""
)

target_compile_features(
    demo_level
    PUBLIC
        cxx_std_17
)

target_compile_definitions(
        demo_level
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(
        demo_level
        PRIVATE
        $<BUILD_INTERFACE:${demo_level_SOURCE_DIR}/include>
)

target_link_libraries(
        demo_level
        PRIVATE
        core::engine
        extensions::support
)

target_sources(
        demo_level
        PRIVATE
        src/entry_point.cpp
)

