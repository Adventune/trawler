/**
 * @file wifi_controller.h
 * @author adventune
 * @date 2023-09-28
 * @copyright Copyright (c) 2023
 *
 * @brief Provides interface for the wifi component
 */

#ifndef TRAWLER_WIFI_H
#define TRAWLER_WIFI_H

#include "esp_wifi.h"

/**
 * @brief Starts AP with given config
 *
 * @param wifi_config
 */
void wifictl_ap_start();

/**
 * @brief Stops running AP
 *
 */
void wifictl_ap_stop();

/**
 * @brief Starts default management AP
 *
 */
void wifictl_mgmt_ap_start();

/**
 * @brief Connects station interface to the given AP
 *
 * @param ap_record
 * @param password password for target network
 */
void wifictl_sta_connect();

/**
 * @brief Disconnects station interface from currently connected AP
 *
 */
void wifictl_sta_disconnect();

/**
 * @brief Sets AP interface MAC address
 *
 * @param mac_ap valid MAC address is expected, array of size 6
 */
void wifictl_set_ap_mac(const uint8_t *mac_ap);

/**
 * @brief Saves current AP interface MAC to given parameter
 *
 * @attention this function expects that the mac_ap points to already allocated
 * memory (6 bytes)
 * @param mac_ap 6 bytes memory block
 */
void wifictl_get_ap_mac(uint8_t *mac_ap);

/**
 * @brief Restores original AP interface MAC that was set during Wi-Fi
 * initialisation.
 */
void wifictl_restore_ap_mac();

/**
 * @brief Sets STA interface MAC address
 *
 * @param mac_ap valid MAC address is expected, array of size 6
 */
void wifictl_get_sta_mac(uint8_t *mac_sta);

/**
 * @brief Sets new channel for Wi-Fi interface
 *
 * @param channel channel in range 1 - 13
 */
void wifictl_set_channel(uint8_t channel);

#endif // TRAWLER_WIFI_H
