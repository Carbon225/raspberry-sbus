target_sources(rcdrivers PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/SBUS.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DecoderFSM.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/packet_decoder.c"
        )

target_include_directories(rcdrivers PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
