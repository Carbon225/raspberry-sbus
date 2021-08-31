add_library(libsbus STATIC)
set_property(TARGET libsbus PROPERTY C_STANDARD 99)
set_property(TARGET libsbus PROPERTY CXX_STANDARD 11)

# old name for compatibility
add_library(sbuslib ALIAS libsbus)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/common")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tty")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/decoder")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/driver")
