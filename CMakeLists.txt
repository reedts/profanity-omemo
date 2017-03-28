cmake_minimum_required (VERSION 2.7)

project (profanity-omemo)

find_library (LIBSIGNAL libsignal-protocol-c.a)

set (BUILD_DIR "${PROJECT_SOURCE_DIR}/lib")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/DEBUG)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/DEBUG)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -std=c99")

add_subdirectory ("${PROJECT_SOURCE_DIR}/src")
