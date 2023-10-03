/**
 * @file attack_wifi.h
 * @auther adventune
 * @date 2023-09-28
 * @copyright Copyright (C) 2023
 *
 * @brief Provides interface for wifi based attacks
 **/
#ifndef ATTACK_WIFI_H
#define ATTACK_WIFI_H

#include "attack.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "frame_analyzer.h"
#include "pcap_serializer.h"
#include "hccapx_serializer.h"
#include "sniffer.h"
#include "wsl_bypasser.h"
#include <cstring>

attack_errors_t attack_wifi_start_handshake();

attack_errors_t attack_wifi_start_dos();

attack_errors_t attack_wifi_stop_handshake();

attack_errors_t attack_wifi_stop_dos();

#endif //TRAWLER_ATTACK_WIFI_H
