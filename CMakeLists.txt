cmake_minimum_required(VERSION 3.16)
project(rpi_sbus)

add_library(sbuslib STATIC
        src/SBUS.cpp src/sbus_driver.c)
target_include_directories(sbuslib PUBLIC include)

add_executable(sbus_test examples/send_to_self.cpp)
target_link_libraries(sbus_test PUBLIC sbuslib)