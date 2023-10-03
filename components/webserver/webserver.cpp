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
#include "esp_event.h"
#include "ap_scanner.h"
#include "attack.h"
#include "pcap_serializer.h"
#include "hccapx_serializer.h"

static const char *TAG = "trawler_webserver";
// Import index_html file
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");

static AttackManager *attack_manager = AttackManager::getInstance();

class HTTPServer {
public:
    HTTPServer() : conf(HTTPD_DEFAULT_CONFIG()) {
    }

    httpd_handle_t StartServer() {
        httpd_handle_t server = NULL;
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.max_uri_handlers = 20;
        config.lru_purge_enable = true;

        // Start the httpd server
        ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
        if (httpd_start(&server, &config) == ESP_OK) {
            this->server = server;
            return server;
        }

        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    void RegisterHandler(const httpd_uri_t *uri_handler) const {
        httpd_register_uri_handler(this->server, uri_handler);
    }

    httpd_handle_t server = nullptr;
    httpd_config_t conf;
};

HTTPServer http;

static void log_server_requests(httpd_req_t *req) {
    ESP_LOGI(TAG, "Request received on URI '%s'", req->uri);
}

// index uri & handler
static const httpd_uri_t uri_index_get = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = [](httpd_req_t *req) {
            log_server_requests(req);
            httpd_resp_send(req, (char *) index_html_start, index_html_end - index_html_start);
            return ESP_OK;
        },
        .user_ctx  = NULL
};

// wifi attack prepare uri
static const httpd_uri_t uri_wifi_attack_prepare_post = {
        .uri      = "/wifi-attack-prepare",
        .method   = HTTP_POST,
        .handler  = [](httpd_req_t *req) {
            log_server_requests(req);
            // Parse attack information from the request
            webserver_attack_wifi_request_t attack_request;
            httpd_req_recv(req, (char *) &attack_request, sizeof(webserver_attack_wifi_request_t));
            esp_err_t res = httpd_resp_send(req, NULL, 0);

            attack_wifi_config_t attack_config = {
                    .type = (attack_wifi_type_t) attack_request.type,
                    .timeout = attack_request.timeout,
                    .ap_record = nullptr
            };
            attack_config.ap_record = wifictl_get_ap_record(attack_request.ap_record_id);
            AttackManager::PrepareAttack(attack_config);
            return res;
        },
        .user_ctx = NULL
};

// attack start uri
static const httpd_uri_t uri_attack_start_post = {
        .uri      = "/attack-start",
        .method   = HTTP_GET,
        .handler  = [](httpd_req_t *req) {
            log_server_requests(req);
            esp_err_t res = httpd_resp_send(req, NULL, 0);
            attack_manager->StartAttack();
            return res;
        },
        .user_ctx = NULL
};

// Get attack content
static const httpd_uri_t uri_attack_content_get = {
        .uri      = "/attack-content",
        .method   = HTTP_GET,
        .handler  = [](httpd_req_t *req) {
            // Commented temporarily for clearer output during development
            // log_server_requests(req);

            ESP_ERROR_CHECK(httpd_resp_set_type(req, HTTPD_TYPE_OCTET));
            // first send attack result header
            ESP_ERROR_CHECK(httpd_resp_send_chunk(req, (char *) &AttackManager::current_attack, 4));
            // send attack result content
            ESP_ERROR_CHECK(
                    httpd_resp_send_chunk(req, AttackManager::current_attack.content, AttackManager::current_attack
                            .content_size));

            return httpd_resp_send_chunk(req, NULL, 0);
        },
        .user_ctx = NULL
};

static const httpd_uri_t uri_capture_hccapx_get = {
        .uri = "/capture.hccapx",
        .method = HTTP_GET,
        .handler = [](httpd_req_t *req) {
            ESP_LOGD(TAG, "Providing HCCAPX file...");
            ESP_ERROR_CHECK(httpd_resp_set_type(req, HTTPD_TYPE_OCTET));
            return httpd_resp_send(req, (char *) hccapx_serializer_get(), sizeof(hccapx_t));
        },
        .user_ctx = NULL
};

static const httpd_uri_t uri_capture_pcap_get = {
        .uri = "/capture.pcap",
        .method = HTTP_GET,
        .handler = [](httpd_req_t *req) {
            log_server_requests(req);
            ESP_LOGD(TAG, "Providing PCAP file...");
            ESP_ERROR_CHECK(httpd_resp_set_type(req, HTTPD_TYPE_OCTET));
            return httpd_resp_send(req, (char *) pcap_serializer_get_buffer(), pcap_serializer_get_size());
        },
        .user_ctx = NULL
};

static const httpd_uri_t uri_ap_list_get = {
        .uri = "/ap-list",
        .method = HTTP_GET,
        .handler = [](httpd_req_t *req) {
            log_server_requests(req);
            wifictl_scan_nearby_aps();

            const wifictl_ap_records_t *ap_records;
            ap_records = wifictl_get_ap_records();

            // 33 SSID + 6 BSSID + 1 RSSI
            char resp_chunk[40];

            ESP_ERROR_CHECK(httpd_resp_set_type(req, HTTPD_TYPE_OCTET));
            for (unsigned i = 0; i < ap_records->count; i++) {
                memcpy(resp_chunk, ap_records->records[i].ssid, 33);
                memcpy(&resp_chunk[33], ap_records->records[i].bssid, 6);
                memcpy(&resp_chunk[39], &ap_records->records[i].rssi, 1);
                ESP_ERROR_CHECK(httpd_resp_send_chunk(req, resp_chunk, 40));
            }
            return httpd_resp_send_chunk(req, resp_chunk, 0);
        },
        .user_ctx = nullptr
};

// Attack stop
static const httpd_uri_t uri_stop_attack = {
        .uri = "/attack-stop",
        .method = HTTP_GET,
        .handler = [](httpd_req_t *req) {
            log_server_requests(req);
            httpd_resp_send(req, nullptr, 0);
            AttackManager::StopAttack();
            return ESP_OK;
        },
        .user_ctx = nullptr
};

static const httpd_uri_t uri_reset_attack = {
        .uri = "/attack-reset",
        .method = HTTP_GET,
        .handler = [](httpd_req_t *req) {
            log_server_requests(req);
            httpd_resp_send(req, nullptr, 0);
            AttackManager::ResetAttack();
            return ESP_OK;
        },
        .user_ctx = nullptr
};

void webserver_start() {
    http.StartServer();
    http.RegisterHandler(&uri_index_get);
    http.RegisterHandler(&uri_capture_pcap_get);
    http.RegisterHandler(&uri_capture_hccapx_get);
    http.RegisterHandler(&uri_wifi_attack_prepare_post);
    http.RegisterHandler(&uri_attack_start_post);
    http.RegisterHandler(&uri_stop_attack);
    http.RegisterHandler(&uri_reset_attack);
    http.RegisterHandler(&uri_attack_content_get);
    http.RegisterHandler(&uri_ap_list_get);
}