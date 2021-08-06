add_executable(send_to_self send_to_self.cpp)
target_link_libraries(send_to_self PUBLIC libsbus)

add_executable(blocking_receiver blocking_receiver.cpp)
target_link_libraries(blocking_receiver PUBLIC libsbus)

add_executable(passthrough passthrough.cpp)
target_link_libraries(passthrough PUBLIC libsbus)

add_executable(simple_send simple_send.cpp)
target_link_libraries(simple_send PUBLIC libsbus)

add_executable(thread_send_receive thread_send_receive.cpp)
target_link_libraries(thread_send_receive PUBLIC libsbus pthread)
