cmake_minimum_required(VERSION 3.9)
project(rpi_sbus VERSION 2.1.0)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

# build examples and tests if this is the main project
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/examples")
    include(CTest)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()
