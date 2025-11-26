# Project Status

## Phase 1: WiFi Provisioning + LED Matrix ✅

### Completed Features

#### 1. Project Structure
- ✅ PlatformIO configuration with ESP-IDF + Arduino framework
- ✅ Proper partition table for NVS storage
- ✅ ESP-IDF default configuration for BLE and WiFi
- ✅ Library dependencies (Adafruit NeoMatrix, NeoPixel, GFX)

#### 2. LED Matrix Control
- ✅ 8x8 WS2812 RGB LED matrix driver (GPIO 14)
- ✅ Text scrolling with color control
- ✅ Status messages:
  - "No WiFi" (orange) - waiting for provisioning
  - "Connecting..." (cyan) - attempting WiFi connection
  - "IP: xxx.xxx.xxx.xxx" (green) - connected and showing IP
  - "Retrying..." (orange) - connection failed, retrying
  - "Error!" (orange) - provisioning error
- ✅ Brightness control (set to 10 to prevent overheating)

#### 3. WiFi Provisioning
- ✅ SoftAP-based provisioning using ESP-IDF
- ✅ Automatic AP naming: `STORY_XXXXXX` (based on MAC)
- ✅ Security 1 with Proof of Possession: `abcd1234`
- ✅ NVS storage for WiFi credentials
- ✅ Auto-reconnect on subsequent boots
- ✅ Event-driven status callbacks
- ✅ Integration with LED matrix for scrolling text feedback
- ✅ IP address retrieval from ESP-IDF (not Arduino WiFi library)
- ✅ Detailed error logging with disconnect reason codes

#### 4. Main Application
- ✅ Initialization sequence
- ✅ Status callback integration
- ✅ Clean serial logging
- ✅ Ready for Phase 2 expansion

### File Structure

```
├── include/
│   ├── audio.h                   # Audio playback API
│   ├── led_matrix.h              # LED matrix API
│   ├── openai_tts.h              # OpenAI TTS API (placeholder)
│   └── wifi_provisioning.h       # WiFi provisioning API
├── src/
│   ├── audio.cpp                 # MP3 playback & HTTPS download
│   ├── main.cpp                  # Main application
│   ├── led_matrix.cpp            # LED matrix implementation
│   ├── openai_tts.cpp            # OpenAI TTS (placeholder)
│   └── wifi_provisioning.cpp     # WiFi provisioning + NTP
├── data/
│   ├── ready.mp3                 # BLE ready sound
│   └── connected.mp3             # WiFi connected sound
├── platformio.ini                # Build configuration
├── partitions.csv                # Flash layout (2MB app, 2MB LittleFS)
├── sdkconfig.defaults            # ESP-IDF/mbedTLS config
└── Documentation files
```

### Testing Status

- ✅ **Complete**: Physical device testing on 2 devices
- ✅ **Complete**: SoftAP provisioning app testing
- ✅ **Complete**: WiFi connection verification
- ✅ **Complete**: LED matrix scrolling text verification
- ✅ **Complete**: IP address display working correctly

### Known Limitations

1. **Hardcoded PoP**: Currently using `abcd1234` - should be unique per device in production
2. **Reset in code**: Currently using `wifi_prov_reset()` in setup for testing - should be removed or made conditional
3. **No timeout handling**: Provisioning waits indefinitely
4. **No factory reset button combo**: Need to erase flash or use code to re-provision
5. **Open AP**: SoftAP has no password (security relies on PoP during provisioning)

## Phase 2: Audio Streaming ✅ COMPLETE

### Completed Features

#### 1. I2S Audio Output
- ✅ I2S driver initialization (GPIO 4, 5, 2)
- ✅ 3W Class D amplifier integration
- ✅ MP3 playback from LittleFS
- ✅ Audio feedback for BLE ready (ready.mp3)
- ✅ Audio feedback for WiFi connected (connected.mp3)

#### 2. HTTPS Audio Streaming
- ✅ HTTPS downloads from Supabase Storage
- ✅ DNS resolution using lwIP (works with ESP-IDF WiFi)
- ✅ WiFiClientSecure with insecure mode
- ✅ Efficient streaming with `writeToStream()`
- ✅ Support for large files (tested with 2MB MP3)
- ✅ Network diagnostics and logging
- ✅ mbedTLS SSL buffer optimization (18KB input buffers)
- ✅ Certificate validation bypass for development

#### 3. MP3 Playback
- ✅ ESP8266Audio library integration
- ✅ MP3 decoding (Helix decoder)
- ✅ Playback from LittleFS
- ✅ Playback from downloaded streams
- ✅ Non-blocking playback in main loop

#### 4. Volume Control
- ✅ Potentiometer-based volume control (GPIO 6)
- ✅ Dynamic volume adjustment (0-100%)
- ✅ Automatic fade-in for streams (20% → 50% over 10s)
- ✅ Smooth linear interpolation
- ✅ Real-time ADC reading with debouncing

#### 5. User Interface
- ✅ Play button control (GPIO 33)
- ✅ Download-then-play workflow
- ✅ Button debouncing
- ✅ Reset button (GPIO 34) - hold 5s to clear WiFi credentials

### Technical Decisions Made

1. **Audio codec**: MP3 using ESP8266Audio library with Helix decoder
2. **Streaming approach**: Download to LittleFS then play (reliable for 2MB+ files)
3. **Buffer size**: 512 bytes for downloads, library handles playback buffering
4. **Sample rate**: 44.1kHz (supports both speech and music)
5. **SSL/TLS**: Insecure mode for development (certificate validation disabled)
6. **Memory**: Optimized for ESP32-S3 without PSRAM (~2.3MB free heap)

## Phase 3: User Interface (Future)

### Planned Features

#### 1. Button Controls
- [ ] GPIO configuration (GPIO 1, 2, 3)
- [ ] Debouncing
- [ ] Button event handling
- [ ] Story selection interface

#### 2. Volume Control
- [ ] Potentiometer ADC reading (GPIO 4)
- [ ] Volume mapping
- [ ] Visual feedback on LED matrix

#### 3. Story Management
- [ ] Story list storage
- [ ] Story selection UI
- [ ] Story metadata display

## Phase 4: Cloud Integration (Future)

### Planned Features

#### 1. User Account
- [ ] Device registration
- [ ] User authentication
- [ ] Account linking

#### 2. Story Service
- [ ] Story catalog API
- [ ] Story download/streaming
- [ ] Personalization

#### 3. OTA Updates
- [ ] Firmware update mechanism
- [ ] Version management
- [ ] Rollback capability

## Hardware Pinout Reference

| Function | GPIO | Status |
|----------|------|--------|
| LED Matrix Data | 14 | ✅ Implemented |
| I2S BCLK | 4 | ✅ Implemented |
| I2S LRCLK | 5 | ✅ Implemented |
| I2S DOUT | 2 | ✅ Implemented |
| Potentiometer (Volume) | 6 | ✅ Implemented |
| Play Button | 33 | ✅ Implemented |
| Reset Button (BOOT) | 34 | ✅ Implemented |
| Button 1 (Select) | 1 | 📋 Planned |
| Button 3 (Next/Prev) | 3 | 📋 Planned |

## Next Steps

1. ✅ **Test Phase 1** on physical hardware - COMPLETE
2. ✅ **Verify** WiFi provisioning works reliably - COMPLETE
3. ✅ **Document** any issues or improvements needed - COMPLETE
4. **Remove/disable** `wifi_prov_reset()` from production code
5. **Plan** Phase 2 implementation details
6. **Research** audio streaming libraries and codecs

## Questions Resolved

1. ✅ **Audio format**: MP3 (using ESP8266Audio with Helix decoder)
2. ✅ **Streaming approach**: Download to LittleFS then play (reliable for large files)
3. ✅ **Story length**: Tested with 2MB+ files, ~2MB LittleFS available
4. ✅ **Sample rate**: 44.1kHz (supports speech and music)
5. ✅ **Caching**: Single file at a time in /temp_stream.mp3

## Open Questions

1. How will story selection work? (cloud catalog, local list, etc.)
2. What cloud service will host the stories?
3. How will user accounts be managed?

---

**Last Updated**: Phase 2 complete - HTTPS streaming working
**Current Phase**: Phase 2 - ✅ COMPLETE
**Next Milestone**: Phase 3 - User Interface (buttons, volume control, story selection)

### Phase 1 Achievements

- Successfully implemented BLE provisioning via SoftAP
- LED matrix displays scrolling status text with color coding
- IP address displayed correctly from ESP-IDF network interface
- Physical button reset (hold BOOT for 5 seconds)
- Tested and working on physical devices
- Comprehensive documentation

### Phase 2 Achievements

- HTTPS downloads from Supabase Storage working
- MP3 playback from LittleFS and downloaded streams
- Potentiometer-based volume control with real-time adjustment
- Play button for on-demand audio playback
- Reset button for WiFi credential clearing
- Automatic fade-in for smooth audio transitions
- Hybrid ESP-IDF/Arduino architecture for WiFi + HTTPS
- DNS resolution using lwIP (compatible with ESP-IDF WiFi)
- Memory optimizations for ESP32-S3 without PSRAM
- mbedTLS SSL buffer tuning for stable HTTPS
- Tested with files up to 2MB
- ~2.3MB free heap during operation

### Latest Changes (Session Summary)

**HTTPS Implementation:**
- Resolved SSL handshake failures (-0x7780, -0x2700 errors)
- Configured mbedTLS with 18KB SSL input buffers
- Implemented certificate validation bypass for development
- Fixed DNS resolution compatibility with ESP-IDF WiFi stack

**Volume Control:**
- Added potentiometer on GPIO 6 (ADC1_CH5)
- Real-time volume adjustment (0-100%)
- Automatic fade functionality for streams
- Debouncing to prevent jitter

**User Interface:**
- Play button on GPIO 33 for on-demand playback
- Download-then-play workflow for better UX
- Reset button on GPIO 34 (hold 5s to clear WiFi)
- Button debouncing for reliable input

**Memory Optimization:**
- Optimized partition layout (2MB app, 2MB LittleFS)
- Removed WAV files, using MP3 for all audio
- Configured SSL buffers for ESP32-S3 without PSRAM
- Maintained ~2.3MB free heap during operation
