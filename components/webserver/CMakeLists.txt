idf_component_register(SRCS "webserver.cpp"
        INCLUDE_DIRS "."
        REQUIRES "hccapx_serializer" "pcap_serializer" "esp_http_server" "attack" "wifi_controller"
        EMBED_TXTFILES "../../data/index.html"
)