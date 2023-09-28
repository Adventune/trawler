/**
 * @file hack_wifi.h
 * @author adventune
 * @date 2023-09-28
 * @copyright Copyright (c) 2023
 *
 * @brief Provides interface for the wifi component
 */

#ifndef ESP_HACK_EPSHACK_WIFI_H
#define ESP_HACK_EPSHACK_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

void hack_wifi_init_softap(void);

#ifdef __cplusplus
}
#endif

#endif //ESP_HACK_EPSHACK_WIFI_H
