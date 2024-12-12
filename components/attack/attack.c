/**
 * @file attack.cpp
 * @auther adventune
 * @date 2023-09-28
 * @copyright Copyright (C) 2023
 *
 * @brief Implements the attack manager
 **/

#include "attack.h"
#include "attack_wifi.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "trawler_attack_manager";

/**
 * @brief Attack manager class
 */

// Initialization
// Private functions and values
attack_errors_t am_start_wifi_attack() {
  ESP_LOGI(TAG, "Starting wifi attack");
  switch (am_current_wifi_config.type) {
  case DEAUTH:
    ESP_LOGI(TAG, "Starting handshake capture attack with deauth");
    return attack_wifi_start_handshake();
  case PASSIVE:
    ESP_LOGI(TAG, "Starting handshake capture attack");
    return attack_wifi_start_handshake();
  case DOS:
  default:
    ESP_LOGE(TAG, "Unknown wifi attack type!");
    return ATTACK_ERROR_UNKNOWN_TYPE;
  }
}

// Public functions and values
attack_status_t current_attack;
attack_wifi_config_t current_wifi_config;

attack_errors_t am_prepare_attack(attack_wifi_config_t attack_config) {
  ESP_LOGI(TAG, "Preparing a wifi attack");

  current_attack.category = WIFI;
  current_wifi_config = attack_config;
  current_attack.state = READY;

  return ATTACK_ERROR_NONE;
}

attack_errors_t am_start_attack() {
  ESP_LOGI(TAG, "Starting attack");
  if (current_attack.state != READY) {
    ESP_LOGE(TAG, "Attack not ready!");
    return ATTACK_ERROR_NOT_READY;
  }

  current_attack.state = RUNNING;

  // Start attack
  switch (current_attack.category) {
  case WIFI:
    return am_start_wifi_attack();
  default:
    ESP_LOGE(TAG, "Unknown attack category!");
    return ATTACK_ERROR_UNKNOWN_CATEGORY;
  }
}

attack_errors_t am_stop_attack() {
  ESP_LOGI(TAG, "Stopping attack");
  if (current_attack.state != RUNNING) {
    ESP_LOGE(TAG, "Attack not running!");
    return ATTACK_ERROR_NONE;
  }

  current_attack.state = FINISHED;

  // Stop attack
  switch (current_attack.category) {
  case WIFI:
    switch (current_wifi_config.type) {
    case DEAUTH:
    case PASSIVE:
      ESP_LOGI(TAG, "Stopping handshake capture attack");
      attack_wifi_stop_handshake();
      break;
    case DOS:
      ESP_LOGI(TAG, "Stopping dos attack");
      attack_wifi_stop_dos();
      break;
    default:
      ESP_LOGE(TAG, "Unknown wifi attack type!");
      return ATTACK_ERROR_UNKNOWN_TYPE;
    }
    return ATTACK_ERROR_NONE;
  default:
    ESP_LOGE(TAG, "Unknown attack category!");
    return ATTACK_ERROR_UNKNOWN_CATEGORY;
  }
}

attack_errors_t am_reset_attack() {
  if (current_attack.state == RUNNING) {
    am_stop_attack();
  }
  current_attack.state = IDLE;
  current_attack.content = NULL;
  current_attack.content_size = 0;

  ESP_LOGI(TAG, "Attack reset");

  return ATTACK_ERROR_NONE;
}

// TODO
void am_append_status_content(uint8_t *buffer, unsigned size) {
  if (size == 0) {
    ESP_LOGE(TAG, "Size can't be 0 if you want to reallocate");
    return;
  }

  // Temporarily save new location in case of realloc failure to preserve
  // current content
  char *reallocated_content = (char *)realloc(
      (void *)current_attack.content, current_attack.content_size + size);

  if (reallocated_content == NULL) {
    ESP_LOGE(TAG, "Error reallocating status content! Status content may not "
                  "be complete.");
    return;
  }

  // Copy new data after current content
  memcpy(&reallocated_content[current_attack.content_size], buffer, size);
  current_attack.content = reallocated_content;
  current_attack.content_size += size;
}

attack_wifi_config_t *am_get_current_wifi_config() {
  return &current_wifi_config;
}
attack_status_t *am_get_current_attack() { return &current_attack; }
