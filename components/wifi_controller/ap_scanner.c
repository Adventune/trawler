/**
 * @file ap_scanner.c
 * @author risinek (risinek@gmail.com)
 * @date 2021-04-05
 * @copyright Copyright (c) 2021
 *
 * @brief Implements AP scanning functionality.
 */
#include "ap_scanner.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"

static const char *TAG = "trawler_ap_scanner";
/**
 * @brief Stores last scanned AP records into linked list.
 *
 */
static wifictl_ap_records_t ap_records;

void wifictl_scan_nearby_aps() {
  ESP_LOGD(TAG, "Scanning nearby APs...");

  ap_records.count =
      10; // if count is changed remember to change it in the header file too

  wifi_scan_config_t scan_config = {.ssid = NULL,
                                    .bssid = NULL,
                                    .channel = 0,
                                    .scan_type = WIFI_SCAN_TYPE_ACTIVE};

  ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
  ESP_ERROR_CHECK(
      esp_wifi_scan_get_ap_records(&ap_records.count, ap_records.records));
  ESP_LOGI(TAG, "Found %u APs.", ap_records.count);
  ESP_LOGD(TAG, "Scan done.");
}

wifictl_ap_records_t *wifictl_get_ap_records() { return &ap_records; }

wifi_ap_record_t *wifictl_get_ap_record(unsigned index) {
  if (index > ap_records.count) {
    ESP_LOGE(TAG, "Index out of bounds! %u records available, but %u requested",
             ap_records.count, index);
    return NULL;
  }
  return &ap_records.records[index];
}
