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

## Network Stack - CRITICAL
**We use ESP-IDF WiFi stack, NOT Arduino WiFi library!**

This has important implications:
- WiFi: ESP-IDF WiFi (`esp_wifi.h`, `esp_netif.h`)
- DNS: Must use lwIP's `getaddrinfo()` - Arduino's `WiFi.hostByName()` DOES NOT WORK
- HTTP: Can use Arduino HTTPClient but DNS must be resolved first via lwIP
- The Arduino WiFi library conflicts with ESP-IDF WiFi provisioning

### DNS Resolution Pattern
When making HTTP/HTTPS requests, ALWAYS resolve DNS using lwIP first:
```cpp
#include <lwip/netdb.h>

struct addrinfo hints = {};
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
struct addrinfo *result = NULL;
int err = getaddrinfo(hostname, "443", &hints, &result);
// Then use WiFiClientSecure/HTTPClient
```

### Common Pitfall
Arduino's `WiFiClientSecure` and `HTTPClient` internally use `hostByName()` which fails with ESP-IDF WiFi. Always pre-resolve DNS before HTTP connections.

## Always Prioritize
1. PlatformIO build system commands
2. ESP-IDF networking APIs (esp_wifi, esp_netif, lwIP)
3. lwIP for DNS resolution (`getaddrinfo`)
4. platformio.ini for configuration
5. ESP-IDF documentation for low-level features
