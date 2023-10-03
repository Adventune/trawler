idf_component_register(SRCS "wsl_bypasser.c"
        INCLUDE_DIRS "."
        REQUIRES esp_wifi esp_timer
)
target_link_libraries(${COMPONENT_LIB} -Wl,-zmuldefs)