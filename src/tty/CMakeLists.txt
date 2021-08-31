target_sources(libsbus PUBLIC
        sbus_tty_linux.c
        sbus_low_latency_linux.c
        sbus_low_latency_none.c
        )

target_include_directories(libsbus PUBLIC include)
