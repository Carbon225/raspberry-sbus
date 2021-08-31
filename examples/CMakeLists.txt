add_executable(send_to_self send_to_self.cpp)
set_property(TARGET send_to_self PROPERTY C_STANDARD 99)
set_property(TARGET send_to_self PROPERTY CXX_STANDARD 11)
target_link_libraries(send_to_self PUBLIC libsbus)

add_executable(blocking_receiver blocking_receiver.cpp)
set_property(TARGET blocking_receiver PROPERTY C_STANDARD 99)
set_property(TARGET blocking_receiver PROPERTY CXX_STANDARD 11)
target_link_libraries(blocking_receiver PUBLIC libsbus)

add_executable(passthrough passthrough.cpp)
set_property(TARGET passthrough PROPERTY C_STANDARD 99)
set_property(TARGET passthrough PROPERTY CXX_STANDARD 11)
target_link_libraries(passthrough PUBLIC libsbus)
