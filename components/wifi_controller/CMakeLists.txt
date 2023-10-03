idf_component_register(SRCS "wifi_controller.c" "sniffer.c" "ap_scanner.c"
        INCLUDE_DIRS "."
        REQUIRES esp_event esp_wifi nvs_flash
)