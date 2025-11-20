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
│   ├── led_matrix.h              # LED matrix API
│   └── wifi_provisioning.h       # WiFi provisioning API
├── src/
│   ├── main.cpp                  # Main application
│   ├── led_matrix.cpp            # LED matrix implementation
│   └── wifi_provisioning.cpp     # WiFi provisioning implementation
├── arduino/                      # Original Arduino examples (reference)
├── platformio.ini                # Build configuration
├── partitions.csv                # Flash layout
├── sdkconfig.defaults            # ESP-IDF config
├── README.md                     # Project documentation
├── TESTING.md                    # Testing guide
├── COMMANDS.md                   # Command reference
└── PROJECT_STATUS.md             # This file
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

## Phase 2: Audio Streaming (Next)

### Planned Features

#### 1. I2S Audio Output
- [ ] I2S driver initialization (GPIO 33, 34, 35)
- [ ] 3W Class D amplifier integration
- [ ] Audio buffer management

#### 2. HTTP Audio Streaming
- [ ] HTTP client for audio streaming
- [ ] Support for MP3/AAC decoding
- [ ] Buffer management for smooth playback

#### 3. Sample Audio Playback
- [ ] Test with sample audio URL
- [ ] Volume control (basic)
- [ ] Play/pause functionality

#### 4. LED Matrix Integration
- [ ] Show "playing" animation
- [ ] Audio level visualization (optional)

### Technical Decisions Needed

1. **Audio codec**: MP3 (ESP32-audioI2S) vs AAC vs WAV
2. **Streaming library**: ESP32-audioI2S vs custom implementation
3. **Buffer size**: Balance between latency and stability
4. **Sample rate**: 16kHz (speech) vs 44.1kHz (music)

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
| I2S BCLK | 33 | 📋 Planned |
| I2S LRCLK | 34 | 📋 Planned |
| I2S DOUT | 35 | 📋 Planned |
| Button 1 (Select) | 1 | 📋 Planned |
| Button 2 (Play/Pause) | 2 | 📋 Planned |
| Button 3 (Next/Prev) | 3 | 📋 Planned |
| Potentiometer (Volume) | 4 | 📋 Planned |

## Next Steps

1. ✅ **Test Phase 1** on physical hardware - COMPLETE
2. ✅ **Verify** WiFi provisioning works reliably - COMPLETE
3. ✅ **Document** any issues or improvements needed - COMPLETE
4. **Remove/disable** `wifi_prov_reset()` from production code
5. **Plan** Phase 2 implementation details
6. **Research** audio streaming libraries and codecs

## Questions to Resolve

1. What audio format will the service provide? (MP3, AAC, WAV, etc.)
2. Will stories be streamed or downloaded?
3. What's the expected story length? (affects buffer sizing)
4. How many stories should be cached locally?
5. What's the target audio quality? (bitrate, sample rate)

---

**Last Updated**: Phase 1 complete and tested
**Current Phase**: Phase 1 - ✅ COMPLETE
**Next Milestone**: Phase 2 - Audio streaming

### Phase 1 Achievements

- Successfully implemented SoftAP provisioning (switched from BLE)
- LED matrix displays scrolling status text with color coding
- IP address displayed correctly from ESP-IDF network interface
- Tested and working on 2 physical devices
- Comprehensive documentation updated
