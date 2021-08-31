target_sources(libsbus PUBLIC
        sbus_driver.c
        sbus_low_latency.c
        )

target_include_directories(libsbus PUBLIC include)
