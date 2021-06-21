cmake_minimum_required(VERSION 3.16)
project(rpi_sbus)

add_library(libsbus STATIC
        src/SBUS.cpp
        src/sbus_driver.c
)

target_include_directories(libsbus PUBLIC include)

# old name for compatibility
add_library(sbuslib ALIAS libsbus)

# build examples if this is the main project
if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    add_subdirectory(examples)
endif()
