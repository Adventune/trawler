/**
 * @file wifi_controller.c
 * @author adventune
 * @date 2023-09-28
 * @copyright Copyright (c) 2023
 *
 * @brief Implements the wifi component
 */

#include "wifi_controller.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include <string.h>

/**
 * Definitions
 */

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *TAG = "trawler_wifi_controller";

/**
 * @brief Stores current state of Wi-Fi interface
 */
static bool wifi_init = false;
static uint8_t original_mac_ap[6];
static int connect_retries = 0;
static EventGroupHandle_t s_wifi_event_group;

static void wifi_event_handler(void *event_handler_arg,
                               esp_event_base_t event_base, int32_t event_id,
                               void *event_data) {

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (connect_retries < 3) {
      esp_wifi_connect();
      connect_retries++;
      ESP_LOGI(TAG, "retry to connect to the AP");
    } else {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGI(TAG, "connect to the AP fail");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    connect_retries = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

/**
 * @brief Initializes Wi-Fi interface into APSTA mode and starts it.
 *
 * @attention This function should be called only once.
 */
static void wifi_apsta_init() {
  ESP_ERROR_CHECK(esp_netif_init());

  esp_netif_create_default_wifi_ap();
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             &wifi_event_handler, NULL));

  // save original AP MAC address
  ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_AP, original_mac_ap));

  ESP_ERROR_CHECK(esp_wifi_start());
  wifi_init = true;
}

void wifictl_ap_start() {
  wifi_config_t wifi_config = {
      .ap = {.ssid = "ESP32",
             .ssid_len = strlen("ESP32"),
             .password = "123456789",
             .max_connection = 5,
             .authmode = WIFI_AUTH_WPA2_PSK},
  };
  ESP_LOGD(TAG, "Starting AP...");
  if (!wifi_init) {
    wifi_apsta_init();
  }

  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
  ESP_LOGI(TAG, "AP started with SSID=%s", wifi_config.ap.ssid);
}

void wifictl_ap_stop() {
  ESP_LOGD(TAG, "Stopping AP...");
  wifi_config_t wifi_config = {
      .ap = {.max_connection = 0},
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
  ESP_LOGD(TAG, "AP stopped");
}

void wifictl_mgmt_ap_start() {
  wifi_apsta_init();
  wifictl_sta_connect();
  wifictl_ap_start();
}

void wifictl_sta_connect() {
  ESP_LOGI(TAG, "Connecting STA to AP...");

  if (!wifi_init) {
    wifi_apsta_init();
  }

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL,
      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL,
      &instance_got_ip));

  wifi_config_t sta_wifi_config = {
      .sta = {.scan_method = WIFI_FAST_SCAN,
              .ssid = CONFIG_WIFI_SSID,
              .password = CONFIG_WIFI_PASSWORD,
              .pmf_cfg.capable = false,
              .pmf_cfg.required = false},
  };

  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_wifi_config));
  ESP_ERROR_CHECK(esp_wifi_connect());
  ESP_LOGI(TAG, "STA conneted to SSID: %s, PASS: %s", CONFIG_WIFI_SSID,
           CONFIG_WIFI_PASSWORD);
}

void wifictl_sta_disconnect() { ESP_ERROR_CHECK(esp_wifi_disconnect()); }

void wifictl_set_ap_mac(const uint8_t *mac_ap) {
  ESP_LOGD(TAG, "Changing AP MAC address...");
  ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_AP, mac_ap));
}

void wifictl_get_ap_mac(uint8_t *mac_ap) {
  esp_wifi_get_mac(WIFI_IF_AP, mac_ap);
}

void wifictl_restore_ap_mac() {
  ESP_LOGD(TAG, "Restoring original AP MAC address...");
  ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_AP, original_mac_ap));
}

void wifictl_get_sta_mac(uint8_t *mac_sta) {
  esp_wifi_get_mac(WIFI_IF_STA, mac_sta);
}

void wifictl_set_channel(uint8_t channel) {
  if ((channel == 0) || (channel > 13)) {
    ESP_LOGE(TAG, "Channel out of range. Expected value from <1,13> but got %u",
             channel);
    return;
  }
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}
