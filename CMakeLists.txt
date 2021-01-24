cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}" -pthread)

project(DodgeTheBlocks)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(DodgeTheBlocks ${SOURCES})
target_link_libraries(DodgeTheBlocks ${CURSES_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(DodgeTheBlocks Threads::Threads)
