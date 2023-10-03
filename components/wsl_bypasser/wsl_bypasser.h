/**
 * @file wsl_bypasser.h
 * @author risinek (risinek@gmail.com)
 * @date 2021-04-05
 * @copyright Copyright (c) 2021
 *
 * @brief Provides interface for Wi-Fi Stack Libraries bypasser
 *
 * This component bypasses blocking mechanism that doesn't allow sending some arbitrary 802.11 frames like deauth etc.
 */
#ifndef WSL_BYPASSER_H
#define WSL_BYPASSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <esp_timer.h>

/**
 * @brief Sends deauthentication frame with forged source AP from given ap_record
 *
 * This will send deauthentication frame acting as frame from given AP, and destination will be broadcast
 * MAC address - \c ff:ff:ff:ff:ff:ff
 *
 * @param deauth_interval_seconds interval in seconds between deauth frames
 * @param ap_record AP record with valid AP information
 */
void wsl_bypasser_init_deauth(const wifi_ap_record_t *ap_record, int deauth_interval_seconds);

// Reset deauth
void wsl_bypasser_reset_deauth();

#ifdef __cplusplus
}
#endif

#endif