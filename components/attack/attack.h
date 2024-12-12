/**
 * @file attack.h
 * @auther adventune
 * @date 2023-09-28
 * @copyright Copyright (C) 2023
 *
 * @brief Provides interface for the attack manager
 */

#ifndef TRAWLER_ATTACK_H
#define TRAWLER_ATTACK_H

#include <esp_wifi.h>
#include <inttypes.h>

/**
 * @brief States of single attack run.
 */
typedef enum {
  IDLE,     ///< attack is not running
  READY,    ///< attack is ready to be started
  RUNNING,  ///< attack is in progress, attack_status_t.content may not be
            ///< consistent.
  FINISHED, ///< attack has finished, attack_status_t.content contains the
            ///< result
} attack_state_t;

/**
 * @brief Implemented attack categories
 */
typedef enum {
  WIFI,
} attack_category_t;

/**
 * @brief Contains current attack status.
 */
typedef struct {
  attack_state_t state;
  attack_category_t category;
  uint16_t content_size;
  char *content;
} attack_status_t;

/**
 * @brief Contains attack class function errors
 */
typedef enum {
  ATTACK_ERROR_NONE,
  ATTACK_ERROR_NOT_READY,        ///< attack was not ready to be started
  ATTACK_ERROR_UNKNOWN_TYPE,     ///< attack type is not implemented
  ATTACK_ERROR_UNKNOWN_CATEGORY, ///< attack category is not implemented
} attack_errors_t;

//* ===================== WIFI ATTACKS =====================

/**
 * @brief Implemented wifi attack types
 *
 */
typedef enum {
  PASSIVE, ///< passive attack, no packets are sent, only sniffing
  DEAUTH,  ///< deauthentication attack, sends deauth packets to the target and
           ///< sniffs for 4-way handshake
  DOS, ///< denial of service attack, sends deauth packets to the target without
       ///< sinffing
} attack_wifi_type_t;

/**
 * @brief Attack config parsed from webserver request
 */
typedef struct {
  attack_wifi_type_t type;
  int timeout;
  wifi_ap_record_t ap_record;
} attack_wifi_config_t;

static attack_errors_t am_start_wifi_attack();

// Current attack status
attack_status_t am_current_attack;

// Current wifi attack config
attack_wifi_config_t am_current_wifi_config;

attack_errors_t am_prepare_attack(attack_wifi_config_t attack_config);

// Starts the prepared attack
attack_errors_t am_start_attack();

// Stops the attack
attack_errors_t am_stop_attack();

// Resets the attack
attack_errors_t am_reset_attack();

// Append new status content to the status
void am_append_status_content(uint8_t *buffer, unsigned size);

attack_wifi_config_t *am_get_current_wifi_config();
attack_status_t *am_get_current_attack();

#endif // TRAWLER_ATTACK_H
