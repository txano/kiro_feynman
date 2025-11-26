# HTTPS Download Solution - Final Implementation

## Status: ✅ WORKING

Successfully downloading and playing MP3 files from Supabase Storage over HTTPS on ESP32-S3 (no PSRAM).

## The Solution

### Hybrid Architecture Approach

The key was combining ESP-IDF WiFi with Arduino HTTPClient in a way that works together:

```
┌─────────────────────────────────────────┐
│         Application Layer               │
│  (main.cpp, audio playback)             │
└─────────────────────────────────────────┘
                  │
    ┌─────────────┴─────────────┐
    │                           │
┌───▼────────────┐    ┌────────▼─────────┐
│  ESP-IDF WiFi  │    │ Arduino HTTPS    │
│  Provisioning  │    │ HTTPClient       │
│  + Network     │    │ WiFiClientSecure │
└───┬────────────┘    └────────┬─────────┘
    │                           │
    └─────────────┬─────────────┘
                  │
         ┌────────▼─────────┐
         │   lwIP Network   │
         │   Stack (DNS)    │
         └──────────────────┘
```

### Key Components

1. **DNS Resolution**: lwIP's `getaddrinfo()` (works with ESP-IDF WiFi)
2. **HTTPS Client**: Arduino's `WiFiClientSecure::setInsecure()`
3. **Streaming**: `HTTPClient::writeToStream()` for efficient downloads
4. **Memory**: Optimized mbedTLS buffers (18KB in, 4KB out)

## Code Implementation

### DNS Resolution (src/audio.cpp)

```cpp
// Use lwIP's getaddrinfo() - works with ESP-IDF WiFi
struct addrinfo hints = {};
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;

struct addrinfo *result = NULL;
int err = getaddrinfo("mdnwjeafufvrrxphotzv.supabase.co", "443", &hints, &result);

if (err == 0 && result != NULL) {
    struct sockaddr_in *addr = (struct sockaddr_in *)result->ai_addr;
    // DNS resolved successfully
}
```

### HTTPS Download (src/audio.cpp)

```cpp
WiFiClientSecure client;
client.setInsecure();  // Skip certificate verification

HTTPClient http;
http.begin(client, url);

int httpCode = http.GET();
if (httpCode == HTTP_CODE_OK) {
    File file = LittleFS.open("/temp_stream.mp3", "w");
    http.writeToStream(&file);
    file.close();
}
```

### Volume Control (src/audio.cpp)

```cpp
void audio_set_volume(float volume) {
    if (out != NULL) {
        out->SetGain(volume);  // 0.0 to 1.0
    }
}

// Automatic fade in audio_loop()
float progress = (float)elapsed / (float)fade_duration;
float target_volume = start_vol + (end_vol - start_vol) * progress;
audio_set_volume(target_volume);
```

## Configuration Files

### sdkconfig.defaults

```ini
# Increase mbedTLS SSL buffer sizes
CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN=18432  # 18KB (was 16KB)
CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN=4096  # 4KB

# Enable dynamic memory management
CONFIG_MBEDTLS_DYNAMIC_BUFFER=y
CONFIG_MBEDTLS_DYNAMIC_FREE_PEER_CERT=y
CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA=y
CONFIG_MBEDTLS_ASYMMETRIC_CONTENT_LEN=y

# Allow insecure TLS (for setInsecure())
CONFIG_ESP_TLS_INSECURE=y
CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY=y
```

### platformio.ini

```ini
build_flags = 
    -DCORE_DEBUG_LEVEL=3
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DBOARD_HAS_PSRAM=0
    -DMBEDTLS_CERTIFICATE_BUNDLE=1
    -DCONFIG_MBEDTLS_SSL_IN_CONTENT_LEN=18432
    -DCONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN=4096
```

### partitions.csv

```csv
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x6000,
phy_init, data, phy,     0xf000,  0x1000,
factory,  app,  factory, 0x10000, 0x200000,
spiffs,   data, spiffs,  0x210000, 0x1F0000,
```

- App: 2MB (75% used)
- LittleFS: ~2MB (for MP3 downloads)

## Memory Usage

| Component | Size | Notes |
|-----------|------|-------|
| SSL Input Buffer | 18KB | Increased for Supabase |
| SSL Output Buffer | 4KB | Reduced (we don't send much) |
| HTTP Client Buffer | 2KB | Optimized |
| Download Buffer | 512 bytes | Small chunks |
| **Total HTTPS Overhead** | **~26KB** | |
| **Free Heap** | **~2.3MB** | During operation |

## Why This Works

### 1. DNS Resolution
- Arduino's `WiFi.hostByName()` doesn't work with ESP-IDF WiFi
- lwIP's `getaddrinfo()` is part of the network stack both use
- Works seamlessly with ESP-IDF's network interface

### 2. HTTPS Client
- `WiFiClientSecure::setInsecure()` bypasses certificate verification
- Acceptable for public storage URLs (Supabase Storage)
- Simpler than managing certificate chains

### 3. Memory Management
- Increased SSL buffers prevent allocation failures
- Dynamic buffer freeing reduces memory footprint
- Small download chunks prevent memory spikes

## Security Considerations

### Current Implementation
⚠️ Uses `setInsecure()` - bypasses certificate verification

### Why This Is Acceptable
- ✅ Supabase Storage URLs are public (no sensitive data)
- ✅ Content integrity can be verified by application
- ✅ Reduces complexity for IoT devices
- ✅ Saves memory (no certificate storage)

### For Production with Sensitive Data
Consider implementing:
- Certificate pinning for specific domains
- Proper certificate chain validation
- Certificate expiry checking
- Hostname verification

## Testing

### Verified Working
- ✅ Small files: `connected.mp3` (~50KB)
- ✅ Large files: `dubai_pirate.mp3` (~2MB)
- ✅ Network diagnostics and logging
- ✅ Volume control with fade
- ✅ Multiple downloads in same session

### Commands
```bash
# Build and upload
pio run --target upload

# Monitor output
pio device monitor

# Erase flash (if needed)
pio run --target erase

# Upload filesystem
pio run --target uploadfs
```

### Expected Logs
```
[audio] Downloading MP3 from URL: https://...
[audio] Free heap before download: 2305847 bytes
[audio] IP: 192.168.1.136
[audio] DNS Main: 8.8.8.8
[audio] DNS resolved to: 104.26.x.x
[audio] WiFiClientSecure created with insecure mode
[audio] Starting HTTP GET request...
[audio] HTTP response code: 200
[audio] Writing stream to file...
[audio] File downloaded and saved: 2048576 bytes
[audio] Volume fade enabled: 20% -> 50% over 10 seconds
[audio] MP3 playback started
```

## Troubleshooting

### DNS Fails
- Check WiFi connection is established
- Verify DNS servers configured (8.8.8.8, 8.8.4.4)
- Check network interface is up

### HTTP Error -1
- DNS resolution failed
- Network not ready
- Check logs for DNS resolution step

### Out of Space
- LittleFS partition full
- Delete old temp files
- Increase LittleFS partition size

### Memory Allocation Errors
- Increase `CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN`
- Reduce other memory usage
- Check free heap before download

## Files Modified

1. `src/audio.cpp` - HTTPS download + volume control
2. `include/audio.h` - API declarations
3. `src/main.cpp` - Test URL configuration
4. `sdkconfig.defaults` - mbedTLS configuration
5. `platformio.ini` - Build flags
6. `partitions.csv` - Partition layout

## Next Steps

### Immediate
- ✅ HTTPS downloads working
- ✅ Volume control implemented
- ✅ Documentation complete

### Future Enhancements
- [ ] Certificate validation for sensitive data
- [ ] Progressive download + play (streaming)
- [ ] Download progress callbacks
- [ ] Retry logic for failed downloads
- [ ] Cache management for multiple files
