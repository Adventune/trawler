/**
 * @file wifi_controller.c
 * @author adventune
 * @date 2023-09-28
 * @copyright Copyright (c) 2023
 *
 * @brief Implements the wifi component
 */

#include "wifi_controller.h"

#include <string.h>
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "nvs_flash.h"

static const char *TAG = "trawler_wifi_controller";

#define ESP_WIFI_SSID      "ESP32"
#define ESP_WIFI_PASS      "123456789"
#define ESP_WIFI_CHANNEL   1
#define MAX_STA_CONN       4

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        ESP_LOGI(TAG, "station "
        MACSTR
        " join, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
        ESP_LOGI(TAG, "station "
        MACSTR
        " leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

void trawler_wifi_init_softap(void) {
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
            .ap = {
                    .ssid = ESP_WIFI_SSID,
                    .ssid_len = strlen(ESP_WIFI_SSID),
                    .channel = ESP_WIFI_CHANNEL,
                    .password = ESP_WIFI_PASS,
                    .max_connection = MAX_STA_CONN,
                    .authmode = WIFI_AUTH_WPA2_PSK,
                    .pmf_cfg = {
                            .required = true,
                    },
            },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "init_softap finished. SSID:%s", ESP_WIFI_SSID);
}