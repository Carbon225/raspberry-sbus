cmake_minimum_required(VERSION 3.16)
project(rpi_sbus VERSION 2.1.0)

add_subdirectory(src)

# build examples and tests if this is the main project
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    add_subdirectory(examples)
    include(CTest)
    add_subdirectory(test)
endif()
