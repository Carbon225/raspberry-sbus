add_library(libsbus STATIC)

# old name for compatibility
add_library(sbuslib ALIAS libsbus)

add_subdirectory(common)
add_subdirectory(low_level)
add_subdirectory(decoder)
add_subdirectory(driver)
