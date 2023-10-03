idf_component_register(SRCS "attack.cpp" "attack_wifi.cpp"
        INCLUDE_DIRS "."
        REQUIRES "wsl_bypasser" "frame_analyzer" "hccapx_serializer" "pcap_serializer"
)