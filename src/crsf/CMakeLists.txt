target_sources(rcdrivers PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/CRSF.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/CRSFDecoder.cpp"
        )

target_include_directories(rcdrivers PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
