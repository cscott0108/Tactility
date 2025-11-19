# UartRelay — Flagship App for Tactility Enhanced

**The app that started it all.** UartRelay is the reason Tactility Enhanced exists—built to unlock the T-Deck's full USB Serial JTAG potential for professional serial debugging and device monitoring.

## Why UartRelay?

Stock Tactility couldn't expose the ESP32-S3's bidirectional USB Serial JTAG interface to apps. UartRelay needed deep firmware-level access, so we forked Tactility, extended the symbol exports, and built this from the ground up. Now you can turn your T-Deck into a portable UART relay station with USB/SD logging—perfect for field debugging, embedded development, and IoT monitoring.

## Features

- **Bidirectional USB Serial JTAG relay** — Full Rx/Tx between UART devices and USB host
- **Real-time device monitoring** — Configurable baud rates, parity, stop bits
- **SD card session logging** — Timestamped capture with auto-flush for reliability
- **Performance optimized** — Batching, local echo, newline-triggered flush for responsive terminals
- **Dynamic log suppression** — Clean relay output without ESP-IDF noise
- **Portable debugging** — Battery-powered, pocket-sized serial toolkit

![screenshot of UartRelay app](../../Documentation/pics/screenshot-UartRelay.png)

## Download & Install

- Prebuilt (direct):
-  - ELF: https://raw.githubusercontent.com/cscott0108/tactility-enhanced-t-deck/main/Prebuilt/SDCard/app/one.tactiity.uartrelay/elf/esp32s3.elf
-  - Manifest: https://raw.githubusercontent.com/cscott0108/tactility-enhanced-t-deck/main/Prebuilt/SDCard/app/one.tactiity.uartrelay/manifest.properties
- Releases (alternative): https://github.com/cscott0108/tactility-enhanced-t-deck/releases
- Source (buildable): https://github.com/cscott0108/tactility-enhanced-t-deck/tree/main/Apps/UartRelay
- SD card layout (ready-to-copy): https://github.com/cscott0108/tactility-enhanced-t-deck/tree/main/Prebuilt/SDCard/app/one.tactiity.uartrelay
- Install: either copy the folder `one.tactiity.uartrelay` into `/app/` on your SD card (preferred), or copy just the `.elf` to `/app/` then launch from the app list

## Usage

1. Open UartRelay on the T-Deck
2. Select UART port and baud rate
3. Toggle the relay to start forwarding between UART and USB Serial JTAG
4. Optionally enable SD logging for session capture (only available when USB relay is off; USB terminals have built-in logging)
5. Press stop to end the relay (cleanup is automatic)

Tips:
- Newline-triggered flush improves small-packet responsiveness
- Local echo can be toggled for interactive sessions

## Build From Source

```bash
cd Apps/UartRelay
python3 tactility.py build esp32s3
```

The build outputs an ELF you can copy to `/sdcard/app/`.

## Troubleshooting

- If you see unexpected logs while relaying, update to the latest firmware/app which suppresses console output during relay
- If SD logs don’t appear, ensure the SD card is present and writable; the app creates/flushes logs periodically

## License

GPLv3 — see repository root `LICENSE.md`.

This project inherits the GPL v3 license from Tactility. All modifications and enhancements are also GPL v3.

## Support

- **Issues**: [GitHub Issues](https://github.com/cscott0108/tactility-enhanced-t-deck/issues)
- **Discussions**: [GitHub Discussions](https://github.com/cscott0108/tactility-enhanced-t-deck/discussions)
- **Original Tactility**: [https://tactility.one](https://tactility.one)

---

**Status**: Active Development | **Version**: 0.7.0-enhanced | **Last Updated**: November 2024
