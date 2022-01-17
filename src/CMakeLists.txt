cmake_minimum_required(VERSION 3.10)

project(oriflamme)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_FILES action.cpp game.cpp main.cpp state.cpp)

add_executable(oriflamme ${SOURCE_FILES})
