/**
 * @file webserver.cpp
 * @author adventune
 * @date 2023-09-28
 * @copyright Copyright (c) 2023
 *
 * @brief Implements the web server
 */

#include "webserver.h"
#include <esp_http_server.h>
#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "trawler_webserver";

class HTTPServer {
public:
    HTTPServer() : conf(HTTPD_DEFAULT_CONFIG()) {
    }

    httpd_handle_t StartServer() {
        httpd_handle_t server = NULL;
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.lru_purge_enable = true;

        // Start the httpd server
        ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
        if (httpd_start(&server, &conf) == ESP_OK) {
            this->server = server;
            return server;
        }

        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    void RegisterHandler(const httpd_uri_t *uri_handler) {
        httpd_register_uri_handler(this->server, uri_handler);
    }

    httpd_handle_t server = nullptr;
    httpd_config_t conf;
};

// Import index_html file
extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");
const size_t index_html_length = index_html_end - index_html_start;

HTTPServer http;

static void log_server_requests(httpd_req_t *req) {
    ESP_LOGI(TAG, "Request received on URI '%s'", req->uri);
}

// index handler
static const httpd_uri_t uri_index_get = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = [](httpd_req_t *req) {
            log_server_requests(req);
            httpd_resp_send(req, index_html_start, index_html_length);
            return ESP_OK;
        },
        .user_ctx  = NULL
};

void trawler_init_server(void) {
    http.StartServer();
    http.RegisterHandler(&uri_index_get);
}