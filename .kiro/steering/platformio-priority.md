# PlatformIO Framework Priority

## Build System
This project uses **PlatformIO** as the build system with the **Arduino framework for ESP32**.

## Key Points
- Platform: `espressif32@6.4.0`
- Framework: `arduino` (Arduino framework for ESP32)
- Board: `esp32-s3-devkitc-1`

## Framework Clarification
While we use the Arduino framework, we're building with PlatformIO, NOT the Arduino IDE. This means:

1. **Use PlatformIO commands**: `pio run`, `pio run --target upload`, `pio device monitor`
2. **Configuration file**: `platformio.ini` (not Arduino IDE settings)
3. **Library management**: PlatformIO's `lib_deps` in platformio.ini
4. **Build flags**: Defined in platformio.ini under `build_flags`

## ESP-IDF vs Arduino
- We use **ESP-IDF APIs** for WiFi provisioning (esp_wifi, esp_event, etc.)
- We use **ESP-IDF HTTP client** (esp_http_client) for network requests
- We use **Arduino APIs** for basic I/O, Serial, and some libraries
- This hybrid approach is common in ESP32 development with PlatformIO

## Network Stack
- WiFi: ESP-IDF WiFi (not Arduino WiFi library)
- DNS: lwIP (ESP-IDF's network stack)
- HTTP: esp_http_client (ESP-IDF)
- The Arduino WiFi library doesn't work properly with ESP-IDF WiFi provisioning

## Always Prioritize
1. PlatformIO build system commands
2. ESP-IDF networking APIs
3. platformio.ini for configuration
4. ESP-IDF documentation for low-level features
