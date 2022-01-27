project(ooio)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

option(ooio-unittest "set to on to build unittests" OFF)
include_directories(include)

add_executable(ooio src/main.cpp)
