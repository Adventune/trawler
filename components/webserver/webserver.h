/**
 * @file webserver.h
 * @author adventune
 * @date 2023-09-28
 * @copyright Copyright (c) 2023
 *
 * @brief Provides interface for the web server
 */

#ifndef TRAWLER_WEBSERVER_H
#define TRAWLER_WEBSERVER_H

#include <esp_http_server.h>
#include "esp_log.h"
#include "esp_system.h"

void trawler_init_server(void);

#endif //TRAWLER_WEBSERVER_H
