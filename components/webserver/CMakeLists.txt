idf_component_register(SRCS "webserver.c"
        INCLUDE_DIRS "."
        REQUIRES hccapx_serializer pcap_serializer esp_http_server attack wifi_controller
)
