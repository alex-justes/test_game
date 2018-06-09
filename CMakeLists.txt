cmake_minimum_required(VERSION 3.10)
project(test_game)

add_subdirectory(sea-engine)


#[[target_include_directories(
        engine
        PUBLIC
        $<BUILD_INTERFACE:${test_game_SOURCE_DIR}/include>
)]]


add_executable(run "")
target_link_libraries(run PRIVATE core::engine)
target_sources(run PRIVATE main.cpp)

add_library(test_level SHARED "")
target_link_libraries(test_level PRIVATE core::engine)
target_sources(test_level PRIVATE test_level.cpp)

