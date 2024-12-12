idf_component_register(SRCS "attack.c" "attack_wifi.c"
        INCLUDE_DIRS "."
        REQUIRES "wsl_bypasser" "frame_analyzer" "hccapx_serializer" "pcap_serializer"
)
