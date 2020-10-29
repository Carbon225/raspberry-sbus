cmake_minimum_required(VERSION 3.16)
project(rpi_sbus)

add_library(sbuslib STATIC
        src/SBUS.cpp src/sbus_driver.c)
target_include_directories(sbuslib PUBLIC include)

add_executable(send_to_self examples/send_to_self.cpp)
target_link_libraries(send_to_self PUBLIC sbuslib)

add_executable(blocking_receiver examples/blocking_receiver.cpp)
target_link_libraries(blocking_receiver PUBLIC sbuslib)

add_executable(passthrough examples/passthrough.cpp)
target_link_libraries(passthrough PUBLIC sbuslib)
