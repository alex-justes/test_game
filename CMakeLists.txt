cmake_minimum_required(VERSION 3.10)
project(test_game)

add_subdirectory(sea-engine)
add_subdirectory(extensions)

add_subdirectory(levels)


add_executable(run "")
target_link_libraries(run PRIVATE core::engine)
target_sources(run PRIVATE main.cpp)


