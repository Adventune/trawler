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

#include "esp_log.h"
#include <string>
#include "esp_wifi_types.h"

/**
 * @brief States of single attack run.
 */
typedef enum {
    IDLE,       ///< attack is not running
    READY,      ///< attack is ready to be started
    RUNNING,    ///< attack is in progress, attack_status_t.content may not be consistent.
    FINISHED,   ///< attack has finished, attack_status_t.content contains the result
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
    char* content;
} attack_status_t;

/**
 * @brief Contains attack class function errors
 */
typedef enum {
    ATTACK_ERROR_NONE,
    ATTACK_ERROR_NOT_READY, ///< attack was not ready to be started
    ATTACK_ERROR_UNKNOWN_TYPE, ///< attack type is not implemented
    ATTACK_ERROR_UNKNOWN_CATEGORY, ///< attack category is not implemented
} attack_errors_t;

//* ===================== WIFI ATTACKS =====================

/**
 * @brief Implemented wifi attack types
 *
 */
typedef enum {
    PASSIVE, ///< passive attack, no packets are sent, only sniffing
    DEAUTH, ///< deauthentication attack, sends deauth packets to the target and sniffs for 4-way handshake
    DOS, ///< denial of service attack, sends deauth packets to the target without sinffing
} attack_wifi_type_t;

/**
 * @brief Attack config parsed from webserver request
 */
typedef struct {
    attack_wifi_type_t type;
    int timeout;
    const wifi_ap_record_t *ap_record;
} attack_wifi_config_t;

class AttackManager {
private:
    /**
     * @brief Starts a wifi attack
     * @return attack_errors_t
     */
    static attack_errors_t StartWifiAttack() ;

    // Pointer to the single instance
    static AttackManager *instancePtr;

    // Default constructor
    AttackManager();

public:
    // Current attack status
    static attack_status_t current_attack;

    // Current wifi attack config
    static attack_wifi_config_t current_wifi_config;

    // deleting copy constructor
    AttackManager(const AttackManager &obj) = delete;

    /*
        getInstance() is a static method that returns an
        instance when it is invoked. It returns the same
        instance if it is invoked more than once as an instance
        of AttackManager class is already created. It is static
        because we have to invoke this method without any object
        of AttackManager class and static method can be invoked
        without object of class

        As constructor is private so we cannot create object of
        AttackManager class without a static method as they can be
        called without objects. We have to create an instance of
        this AttackManager class by using getInstance() method.
    */
    static AttackManager *getInstance();

    /**
     * @brief Prepares an attack
     * @param attack_type
     * @param attack_config
     *
     * Function is overloaded to support different attack categories
     *
     * @return attack_errors_t
     */
    static attack_errors_t PrepareAttack(attack_wifi_config_t attack_config);

    // Starts the prepared attack
    static attack_errors_t StartAttack();

    // Stops the attack
    static attack_errors_t StopAttack();

    // Resets the attack
    static attack_errors_t ResetAttack();

    // Append new status content to the status
    static void AppendStatusContent(uint8_t *buffer, unsigned size);
};

#endif //TRAWLER_ATTACK_H
