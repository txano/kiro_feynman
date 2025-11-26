# Changelog

All notable changes to the Story Device project will be documented in this file.

## [2.1.0] - 2025-11-26

### Changed - Code Cleanup & Documentation Update

#### Code Cleanup
- Removed unused `audio_play_mp3()` wrapper function from audio.cpp
- Renamed misleading `test_openai_tts` variable to `download_test_file` in main.cpp
- Removed unused `openai_tts.h` include from main.cpp
- Removed commented-out OpenAI TTS initialization code

#### Documentation Consolidation
- Deleted redundant HTTPS documentation files:
  - `HTTPS_SOLUTION_SUMMARY.md` (merged into HTTPS_CERTIFICATE_STATUS.md)
  - `HTTPS_FIX_SUMMARY.md` (merged into HTTPS_CERTIFICATE_STATUS.md)
  - `HTTPS_TLS_NOTES.md` (merged into HTTPS_CERTIFICATE_STATUS.md)
- Deleted outdated phase documentation:
  - `PHASE1_CHANGES.md` (info in CHANGELOG.md)
  - `PHASE1_COMPLETE.md` (info in PROJECT_STATUS.md)
  - `PHASE2_AUDIO.md` (info in PROJECT_STATUS.md)

#### Documentation Updates
- Updated README.md with current Phase 2 status and GPIO pinout
- Updated ARCHITECTURE.md with current audio implementation
- Updated PROJECT_STATUS.md with resolved questions
- Updated QUICKSTART.md and QUICK_REFERENCE.md with current phase status
- Updated product.md steering file with current features
- Updated TODO.md with accurate known issues

## [2.0.0] - 2024-11-26

### Added - Phase 2 Complete: Audio Streaming & User Interface

#### HTTPS Audio Streaming
- HTTPS download support from Supabase Storage
- DNS resolution using lwIP (compatible with ESP-IDF WiFi stack)
- WiFiClientSecure with certificate validation bypass for development
- mbedTLS SSL buffer optimization (18KB input buffers)
- Download-then-play workflow for reliable large file handling
- Support for files up to 2MB tested successfully

#### Volume Control System
- Potentiometer-based volume control on GPIO 6 (ADC1_CH5)
- Real-time volume adjustment (0-100% range)
- Automatic fade-in functionality for smooth audio transitions
- Configurable fade parameters (start volume, end volume, duration)
- ADC reading with debouncing to prevent jitter
- Volume updates every 200ms with 2% threshold for changes

#### User Interface Controls
- Play button on GPIO 33 for on-demand audio playback
- Reset button on GPIO 34 (BOOT button) - hold 5 seconds to clear WiFi credentials
- Button debouncing (50ms) for reliable input
- Download-then-play workflow: file downloads in background, plays on button press
- Visual feedback via LED matrix for all operations

#### Memory Optimization
- Optimized partition layout: 2MB app, 2MB LittleFS
- Removed WAV files, using MP3 format exclusively
- Configured mbedTLS SSL buffers for ESP32-S3 without PSRAM
- Maintained ~2.3MB free heap during operation
- Efficient streaming with 512-byte download buffer

#### Audio Enhancements
- MP3 playback from LittleFS using ESP8266Audio library
- Helix MP3 decoder integration
- Non-blocking audio playback in main loop
- Audio status checking (is_playing)
- Support for 44.1kHz sample rate (speech and music)

### Changed

#### Architecture Improvements
- Hybrid ESP-IDF/Arduino architecture for WiFi + HTTPS
- Separated download and playback functions
- Improved error handling and logging
- Better memory management for large files

#### Configuration Updates
- Updated `sdkconfig.defaults` with mbedTLS optimizations
- Modified `platformio.ini` with ESP8266Audio library
- Optimized partition table in `partitions.csv`
- Added build flags for SSL buffer configuration

### Fixed

#### SSL/TLS Issues
- Resolved SSL handshake failure error -0x7780 (MBEDTLS_ERR_SSL_CONN_EOF)
- Fixed SSL handshake failure error -0x2700 (MBEDTLS_ERR_SSL_TIMEOUT)
- Corrected DNS resolution compatibility with ESP-IDF WiFi
- Fixed Arduino WiFi library conflicts with ESP-IDF stack

#### Memory Issues
- Resolved heap fragmentation during HTTPS downloads
- Fixed SSL buffer allocation failures
- Optimized partition layout for available flash space

#### User Experience
- Improved button responsiveness with debouncing
- Fixed volume control jitter with threshold filtering
- Better audio transition with fade-in functionality

### Technical Details

#### Modified Files
- `src/audio.cpp` - Complete rewrite for HTTPS and volume control
- `src/main.cpp` - Added button handlers and volume control
- `include/audio.h` - Updated API with new functions
- `include/wifi_provisioning.h` - Added time sync function
- `src/wifi_provisioning.cpp` - Implemented NTP time sync
- `platformio.ini` - Added ESP8266Audio library
- `sdkconfig.defaults` - Optimized mbedTLS configuration
- `partitions.csv` - Optimized partition layout

#### New Files
- `HTTPS_CERTIFICATE_STATUS.md` - SSL/TLS troubleshooting documentation
- `HTTPS_FIX_SUMMARY.md` - Summary of HTTPS fixes
- `HTTPS_SOLUTION_SUMMARY.md` - Comprehensive HTTPS solution guide
- `HTTPS_TLS_NOTES.md` - Technical notes on TLS implementation
- `data/connected.mp3` - WiFi connected audio feedback
- `include/openai_tts.h` - OpenAI TTS integration (placeholder)
- `src/openai_tts.cpp` - OpenAI TTS implementation (placeholder)
- `.kiro/steering/platformio-priority.md` - Build system documentation

#### Deleted Files
- `data/connected.wav` - Replaced with MP3 version
- `data/ready.wav` - Using MP3 version instead

### Hardware Configuration

#### GPIO Assignments
- GPIO 14: LED Matrix Data (WS2812)
- GPIO 4: I2S BCLK
- GPIO 5: I2S LRCLK
- GPIO 2: I2S DOUT
- GPIO 6: Volume Potentiometer (ADC1_CH5)
- GPIO 33: Play Button (active LOW)
- GPIO 34: Reset Button / BOOT (active LOW, hold 5s)

#### Memory Layout
- App Partition: 2MB (factory)
- LittleFS: 2MB (spiffs)
- NVS: 20KB
- OTA Data: 8KB
- Free Heap: ~2.3MB during operation

### Known Limitations

- HTTPS certificate validation disabled (insecure mode for development)
- OpenAI API key hardcoded in source code
- No visual volume indicator on LED matrix yet
- No story selection UI implemented yet
- Single file playback only (no playlist support)

### Testing

- Tested on ESP32-S3-Matrix hardware
- Verified HTTPS downloads from Supabase Storage
- Tested with MP3 files up to 2MB
- Confirmed volume control responsiveness
- Verified button debouncing and reset functionality
- Tested WiFi credential reset via button hold

---

## [0.1.0] - Phase 1 Complete - 2024-11-20

### Added - Phase 1: WiFi Provisioning & LED Matrix

#### WiFi Provisioning
- SoftAP-based WiFi provisioning using ESP-IDF
- Automatic AP naming: `STORY_XXXXXX` (based on MAC address)
- Security 1 with Proof of Possession: `abcd1234`
- NVS storage for WiFi credentials
- Auto-reconnect on subsequent boots
- Event-driven status callbacks

#### LED Matrix Display
- 8x8 WS2812 RGB LED matrix driver (GPIO 14)
- Text scrolling with color control
- Status messages with color coding:
  - "No WiFi" (orange) - waiting for provisioning
  - "Connecting..." (cyan) - attempting connection
  - "IP: xxx.xxx.xxx.xxx" (green) - connected with IP
  - "Retrying..." (orange) - connection failed
  - "Error!" (orange) - provisioning error
- Brightness control (set to 10 to prevent overheating)

#### Project Structure
- PlatformIO configuration with ESP-IDF + Arduino framework
- Proper partition table for NVS storage
- ESP-IDF default configuration for BLE and WiFi
- Library dependencies (Adafruit NeoMatrix, NeoPixel, GFX)

#### Documentation
- Comprehensive README with project overview
- ARCHITECTURE.md with system design details
- COMMANDS.md with quick command reference
- QUICKSTART.md for getting started
- TESTING.md with testing procedures
- TODO.md with future work items

### Testing
- Physical device testing on 2 ESP32-S3-Matrix boards
- SoftAP provisioning app testing (Android/iOS)
- WiFi connection verification
- LED matrix scrolling text verification
- IP address display working correctly

