add_library(libsbus STATIC)
set_property(TARGET libsbus PROPERTY C_STANDARD 99)
set_property(TARGET libsbus PROPERTY CXX_STANDARD 11)

# old name for compatibility
add_library(sbuslib ALIAS libsbus)

add_subdirectory(common)
add_subdirectory(low_level)
add_subdirectory(decoder)
add_subdirectory(driver)
