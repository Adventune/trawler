/**
 * @file hack_server.h
 * @author adventune
 * @date 2023-09-28
 * @copyright Copyright (c) 2023
 *
 * @brief Provides interface for the web server
 */
#ifndef ESP_HACK_ESP_HACK_SERVER_H
#define ESP_HACK_ESP_HACK_SERVER_H

#include <esp_http_server.h>
#include "esp_log.h"
#include "esp_system.h"

void hack_init_server(void);

#endif //ESP_HACK_ESP_HACK_SERVER_H
