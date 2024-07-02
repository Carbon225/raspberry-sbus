target_sources(rcdrivers PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/tty_linux.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/tty_dummy.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/tty_low_latency_linux.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/tty_low_latency_none.c"
        )

target_include_directories(rcdrivers PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
