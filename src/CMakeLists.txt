add_library(rcdrivers STATIC)
set_property(TARGET rcdrivers PROPERTY C_STANDARD 99)
set_property(TARGET rcdrivers PROPERTY CXX_STANDARD 11)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/common")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tty")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/sbus")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/crsf")
