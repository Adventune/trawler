idf_component_register(SRCS "main.c"
        INCLUDE_DIRS "."
        REQUIRES nvs_flash wifi_controller webserver esp_timer esp_event
)
