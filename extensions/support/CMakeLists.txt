cmake_minimum_required(VERSION 3.10)
project(support)

add_library(
        support
        SHARED
        ""
)

add_library(extensions::support ALIAS support)

target_compile_features(
    support
    PUBLIC
        cxx_std_17
)

target_compile_definitions(
        support
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(
        support
        PUBLIC
        $<BUILD_INTERFACE:${support_SOURCE_DIR}/include>
)

target_link_libraries(
        support
        PRIVATE
        core::engine
)

target_sources(
        support
        PRIVATE
        include/support/Types.h
        src/Types.cpp
        include/support/Utilities.h
        src/Utilities.cpp
        include/support/Behaviors.h
        include/support/Objects.hpp
        include/support/Actors.h
        src/Objects.cpp
        src/Behaviors.cpp
)

