# Trawler

An ESP32 based knockoff of the Flipper Zero. (With totally different
functionality). Imagine a broke university student getting inspired by a $170
device and trying to make a $10 version of it.

My plans are to cram as many offensive tools as possible to the chip and then
design a PCB-board for this project. All the components and wiring can be found
in [docs](./docs/COMPONENTS_WIRING.md).

## Features

- WiFi attacks
  - DOS
  - Handshake capture
    - Passive listening works

## Credits

I am not smart enough to come up with this on my own. My aim is to bundle as
many offensive tools as possible to the ESP32 chip.

Some code I do modify and create my self but as long as the code is for the most
part not written by me, I try to keep the credits in the code files and listed
here.

### Credists list

- [Most of the WiFi attack implementation (risinek)](https://github.com/risinek/esp32-wifi-penetration-tool/tree/master)
