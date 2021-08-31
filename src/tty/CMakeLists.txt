target_sources(libsbus PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/sbus_tty_linux.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/sbus_low_latency_linux.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/sbus_low_latency_none.c"
        )

target_include_directories(libsbus PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
