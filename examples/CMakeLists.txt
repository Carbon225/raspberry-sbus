add_executable(send_to_self send_to_self.cpp)
target_link_libraries(send_to_self PUBLIC sbuslib)

add_executable(blocking_receiver blocking_receiver.cpp)
target_link_libraries(blocking_receiver PUBLIC sbuslib)

add_executable(passthrough passthrough.cpp)
target_link_libraries(passthrough PUBLIC sbuslib)
