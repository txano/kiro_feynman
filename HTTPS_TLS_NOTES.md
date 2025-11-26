# HTTPS/TLS Configuration Notes

## Current Status

### ✅ Working
- ESP-IDF WiFi provisioning via SoftAP
- DNS resolution using lwIP (ESP-IDF's network stack)
- HTTP client functionality
- MP3 download and playback from LittleFS
- File management (1432 KB free space available)

### ❌ Not Working
- HTTPS connections with certificate verification disabled

## The Problem

ESP-IDF's mbedTLS library requires explicit configuration to skip certificate verification. The Arduino framework for ESP32 doesn't expose a simple API for this.

Error: `ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED`

## Attempted Solutions

1. ✗ `config.skip_cert_common_name_check = true` - Not sufficient
2. ✗ `config.cert_pem = NULL` - Causes "No server verification option set" error  
3. ✗ `config.cert_pem = ""` - Tries to parse empty string as certificate
4. ✗ `config.use_global_ca_store = false` - Still requires verification mode
5. ✗ sdkconfig options - Not used by Arduino framework in PlatformIO

## Working Solutions

### Option 1: Use Valid Certificates
For production APIs like OpenAI that have valid SSL certificates, the default configuration should work once we add the certificate bundle.

### Option 2: HTTP Proxy/CDN
Use a service that provides HTTP access to HTTPS resources.

### Option 3: ESP-IDF Native (Not Arduino)
Switch from Arduino framework to pure ESP-IDF, which gives full control over TLS configuration.

### Option 4: Certificate Bundle
Add the certificate bundle to validate legitimate HTTPS sites:

```cpp
extern const uint8_t server_cert_pem_start[] asm("_binary_server_cert_pem_start");
extern const uint8_t server_cert_pem_end[]   asm("_binary_server_cert_pem_end");

config.cert_pem = (const char *)server_cert_pem_start;
```

## Recommendation for Your Project

Since your goal is OpenAI TTS integration:

1. **OpenAI has valid SSL certificates** - We should be able to connect once we add the CA certificate bundle
2. **The download-then-play approach works** - We've proven this with HTTP
3. **You have sufficient storage** - 1432 KB free is enough for TTS responses

## Next Steps

1. Add ESP32's default certificate bundle for HTTPS validation
2. Test OpenAI TTS API with proper certificates
3. If that fails, implement a simple HTTP proxy for development/testing

## Code Location

- Download function: `src/audio.cpp::audio_play_mp3_url()`
- TTS integration: `src/openai_tts.cpp`
- Main test: `src/main.cpp` (line ~250)
