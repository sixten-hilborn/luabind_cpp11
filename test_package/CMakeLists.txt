PROJECT(TestLuabind11)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

SET(VERBOSE 1)
SET(CMAKE_VERBOSE_MAKEFILE 1)
SET(CMAKE_SKIP_RPATH 0)

add_executable(test_package example.cpp)
target_link_libraries(test_package ${CONAN_LIBS})

# Copy .lua file into executable dir
configure_file(greet.lua greet.lua COPYONLY)
