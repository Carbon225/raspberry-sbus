cmake_minimum_required(VERSION 3.9)
project(rpi_sbus VERSION 2.1.1)

option(USE_TTY_IMPL_DUMMY "Use dummy implementation of the driver" OFF)

if (USE_TTY_IMPL_DUMMY)
    add_definitions(-DRPISBUS_TTY_IMPL -DRPISBUS_TTY_IMPL_DUMMY)
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

# build examples and tests if this is the main project
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/examples")
    include(CTest)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()
