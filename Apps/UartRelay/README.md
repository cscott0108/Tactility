# UartRelay

Bidirectional UART ↔ USB relay app for Tactility Enhanced on LilyGO T-Deck.

## Features

- USB Serial JTAG relay (Rx/Tx)
- UART device monitoring and control
- SD card logging with timestamps
- Configurable baud rates and settings
- Local echo, batching, and newline flush for responsiveness

![screenshot of UartRelay app](../../Documentation/pics/screenshot-UartRelay.png)

## Download & Install

- Prebuilt: download `UartRelay.elf` from project Releases: https://github.com/cscott0108/tactility-enhanced-t-deck/tree/main/Prebuilt/Apps/UartRelay/one.tactility.uartrelay/
- Copy to SD card: place `.elf` in `/sdcard/app/`
- Launch from the Tactility app list on the device

## Usage

1. Open UartRelay on the T-Deck
2. Select UART port and baud rate
3. Toggle the relay to start forwarding between UART and USB Serial JTAG
4. Optionally enable SD logging for session capture
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
