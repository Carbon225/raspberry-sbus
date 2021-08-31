target_sources(libsbus PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/DecoderFSM.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/packet_decoder.c"
        )

target_include_directories(libsbus PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
