# Technology Stack

## Build System
- **Platform**: PlatformIO
- **Framework**: Arduino (ESP32)
- **Platform Version**: espressif32@6.4.0
- **Board**: esp32-s3-devkitc-1

## Core Libraries
- **Adafruit NeoMatrix** (^1.3.0) - LED matrix control
- **Adafruit NeoPixel** (^1.12.0) - WS2812 LED driver
- **Adafruit GFX Library** (^1.11.9) - Graphics primitives
- **ESP-IDF** - WiFi provisioning, event handling, NVS storage

## Hardware Configuration
- **Flash**: 4MB (DIO mode)
- **PSRAM**: Not available
- **Partitions**: Custom (see partitions.csv)
- **USB CDC**: Enabled on boot

## Common Commands

### Build & Flash
```bash
pio run                          # Build only
pio run --target upload          # Build and upload
pio run --target upload && pio device monitor  # Upload and monitor
pio device monitor               # Monitor serial output
```

### Clean & Reset
```bash
pio run --target clean           # Clean build
pio run --target erase           # Erase flash (removes WiFi credentials)
```

### Debugging
```bash
pio device monitor --filter esp32_exception_decoder  # Monitor with decoder
pio device list                  # List connected devices
```

### Dependencies
```bash
pio pkg update                   # Update all dependencies
pio pkg install -l "library"     # Install specific library
```

## Build Flags
- `CORE_DEBUG_LEVEL=3` - Verbose ESP-IDF logging
- `ARDUINO_USB_CDC_ON_BOOT=1` - USB serial on boot
- `BOARD_HAS_PSRAM=0` - No PSRAM available

## Serial Configuration
- **Baud Rate**: 115200
- **Upload Speed**: 921600
