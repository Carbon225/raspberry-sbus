target_sources(libsbus PUBLIC
        DecoderFSM.cpp
        packet_decoder.c
        )

target_include_directories(libsbus PUBLIC include)
