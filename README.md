# ESP-HACK (knockoff zero)

An ESP32 based knockoff of the Flipper Zero. (With totally different functionality)

## Features

- WiFi deauth attacks
    - DOS & handshake capture

## Development

### Pre-requisites

#### Software

- [ESP-IDF & It's pre-requisites](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

#### Hardware

- ESP32 board

### "Deploying" on ESP32 chip

```bash
. ${IDF_PATH}/export.sh # If you haven't already
idf.py build flash monitor
```