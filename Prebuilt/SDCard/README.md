# Prebuilt Apps for Tactility Enhanced

**Ready-to-copy app folders optimized for LilyGO T-Deck.**

This directory contains prebuilt apps for Tactility Enhanced, packaged in the exact SD card folder structure the firmware expects. No build tools required—just copy and run.

## Featured App: UartRelay

**The flagship app that sparked this firmware fork.** UartRelay transforms your T-Deck into a professional serial debugging tool with bidirectional USB Serial JTAG relay, SD logging, and field-ready portability.

**Hardware note:** To relay UART data from external devices, you'll need a JST-PH 2.0mm 4-pin connector (HY2.0-4P) to access the T-Deck's onboard UART port.

## Quick Install

1. Insert your SD card into your computer
2. Copy the folder `one.tactiity.uartrelay` to `/app/` on the SD card root
3. Safely eject the SD card and insert it into your T-Deck
4. Power on or reboot the device
5. Launch UartRelay from the Tactility app list

## Technical Details

**Folder structure:**
- `one.tactiity.uartrelay/elf/esp32s3.elf` — Compiled app binary for ESP32-S3
- `one.tactiity.uartrelay/manifest.properties` — App metadata and configuration

**Important:**
- Do not rename the `one.tactiity.uartrelay` folder—Tactility matches apps by this identifier
- The folder must be placed at `/app/one.tactiity.uartrelay` on the SD card root
- Manual ELF-only installs work but lose metadata; use the full folder for best results

Troubleshooting:
- If the app does not appear, ensure the path is exactly `/app/one.tactiity.uartrelay` with correct spelling
- Make sure the SD card is formatted FAT32 and writable
- Re-seat the SD card and reboot the device if the app list does not refresh
