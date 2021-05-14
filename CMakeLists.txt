cmake_minimum_required(VERSION 3.16)
project(rpi_sbus)

add_library(sbuslib STATIC
        src/SBUS.cpp src/sbus_driver.c)
target_include_directories(sbuslib PUBLIC include)

# build examples if this is the main project
if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    add_subdirectory(examples)
endif()
