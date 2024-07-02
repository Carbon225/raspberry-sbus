add_executable(send_to_self "${CMAKE_CURRENT_SOURCE_DIR}/send_to_self.cpp")
set_property(TARGET send_to_self PROPERTY C_STANDARD 99)
set_property(TARGET send_to_self PROPERTY CXX_STANDARD 11)
target_link_libraries(send_to_self PUBLIC rcdrivers)

add_executable(blocking_receiver "${CMAKE_CURRENT_SOURCE_DIR}/blocking_receiver.cpp")
set_property(TARGET blocking_receiver PROPERTY C_STANDARD 99)
set_property(TARGET blocking_receiver PROPERTY CXX_STANDARD 11)
target_link_libraries(blocking_receiver PUBLIC rcdrivers)

add_executable(passthrough "${CMAKE_CURRENT_SOURCE_DIR}/passthrough.cpp")
set_property(TARGET passthrough PROPERTY C_STANDARD 99)
set_property(TARGET passthrough PROPERTY CXX_STANDARD 11)
target_link_libraries(passthrough PUBLIC rcdrivers)

add_executable(crsf_receiver "${CMAKE_CURRENT_SOURCE_DIR}/crsf_receiver.cpp")
set_property(TARGET crsf_receiver PROPERTY C_STANDARD 99)
set_property(TARGET crsf_receiver PROPERTY CXX_STANDARD 11)
target_link_libraries(crsf_receiver PUBLIC rcdrivers)
