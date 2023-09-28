/**
 * @file main.cpp
 * @author adventune
 * @date 2023-09-28
 * @copyright Copyright (c) 2023
 *
 * @brief Main entry point for the application
 * @details This file is the main entry point for the application. It initializes the wifi component and the web server.
 */

#include "esp_mac.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "../components/hack-wifi/hack-wifi.h"{
#include "../components/hack-server/hack_server.h"

// Define the log tag
static const char *TAG = "hack-main";
extern "C" void app_main(void)
    ESP_LOGI(TAG, "Booting up!");

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    hack_wifi_init_softap();
    hack_init_server();
}