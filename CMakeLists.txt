cmake_minimum_required(VERSION 3.9)
project(rcdrivers VERSION 3.0.0)

option(USE_TTY_IMPL_DUMMY "Use dummy implementation of the driver" OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

if (USE_TTY_IMPL_DUMMY)
    message(WARNING "Using dummy implementation of the driver")
    target_compile_definitions(rcdrivers PUBLIC RPISBUS_TTY_IMPL RPISBUS_TTY_IMPL_DUMMY)
endif()

# build examples and tests if this is the main project
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/examples")
    include(CTest)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()
