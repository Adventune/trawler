idf_component_register(SRCS "hack_server.cpp"
        INCLUDE_DIRS "."
        REQUIRES esp_http_server
        EMBED_TXTFILES "../../data/index.html"
)