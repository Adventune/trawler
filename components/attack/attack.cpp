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
#include <string>

static const char *TAG = "trawler_attack_manager";

/**
 * @brief Attack manager class
 */

// Initialization
AttackManager::AttackManager() = default;

// Private functions and values
attack_errors_t AttackManager::StartWifiAttack() {
    ESP_LOGI(TAG, "Starting wifi attack");
    switch (AttackManager::current_wifi_config.type) {
        case DEAUTH:
        case PASSIVE:
            ESP_LOGI(TAG, "Starting handshake capture attack");
            return attack_wifi_start_handshake();
        case DOS:
            ESP_LOGI(TAG, "Starting dos attack");
            return attack_wifi_start_dos(AttackManager::current_wifi_config);
        default:
            ESP_LOGE(TAG, "Unknown wifi attack type!");
            return ATTACK_ERROR_UNKNOWN_TYPE;
    }
}

// Public functions and values
attack_status_t AttackManager::current_attack;
attack_wifi_config_t AttackManager::current_wifi_config;

attack_errors_t AttackManager::PrepareAttack(attack_wifi_config_t attack_config) {
    ESP_LOGI(TAG, "Preparing a wifi attack");

    current_attack.category = WIFI;
    current_wifi_config = attack_config;
    current_attack.state = READY;

    return ATTACK_ERROR_NONE;
}

attack_errors_t AttackManager::StartAttack() {
    ESP_LOGI(TAG, "Starting attack");
    if (current_attack.state != READY) {
        ESP_LOGE(TAG, "Attack not ready!");
        return ATTACK_ERROR_NOT_READY;
    }

    current_attack.state = RUNNING;

    // Start attack
    switch (current_attack.category) {
        case WIFI:
            return StartWifiAttack();
        default:
            ESP_LOGE(TAG, "Unknown attack category!");
            return ATTACK_ERROR_UNKNOWN_CATEGORY;
    }
}

attack_errors_t AttackManager::StopAttack(){
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

attack_errors_t AttackManager::ResetAttack() {
    if(current_attack.state == RUNNING) {
        StopAttack();
    }
    current_attack.state = IDLE;
    current_attack.content = nullptr;
    current_wifi_config = {};
    current_attack.content_size = 0;
    ESP_LOGI(TAG, "Attack reset");

    return ATTACK_ERROR_NONE;
}

AttackManager *AttackManager::getInstance() {
    // If there is no instance of class then we can create an instance.
    if (instancePtr == nullptr) {
        // We can access private members within the class.
        instancePtr = new AttackManager();
        // returning the instance pointer
        return instancePtr;
    } else {
        // if instancePtr != NULL that means
        // the class already have an instance.
        // So, we are returning that instance
        // and not creating new one.
        return instancePtr;
    }
}

void AttackManager::AppendStatusContent(uint8_t *buffer, unsigned size) {
    if(size == 0){
        ESP_LOGE(TAG, "Size can't be 0 if you want to reallocate");
        return;
    }
    // temporarily save new location in case of realloc failure to preserve current content
    char *reallocated_content = static_cast<char *>(realloc(reinterpret_cast<void *>(current_attack.content),
                                                            current_attack.content_size + size));
    if(reallocated_content == nullptr){
        ESP_LOGE(TAG, "Error reallocating status content! Status content may not be complete.");
        return;
    }
    // copy new data after current content
    memcpy(&reallocated_content[current_attack.content_size], buffer, size);
    current_attack.content = reallocated_content;
    current_attack.content_size += size;
}

// initializing instancePtr with NULL
AttackManager *AttackManager::instancePtr = nullptr;