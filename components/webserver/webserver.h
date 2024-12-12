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

#include <inttypes.h>

void webserver_start();

typedef struct {
  uint8_t ap_record_id; //< ID of chosen AP. It can be used to access ap_records
                        // array from wifi_controller - ap_scanner
  uint8_t type;         //< Chosen type of attack
  uint8_t timeout;      //< Attack timeout in seconds
} webserver_attack_wifi_request_t;

#endif // TRAWLER_WEBSERVER_H
