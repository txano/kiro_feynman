# Product Overview

Story Device for Kids - An ESP32-S3 based storytelling device with LED matrix display and audio streaming capabilities.

## Hardware Platform
- **Board**: Waveshare ESP32-S3-Matrix
- **Display**: 8x8 WS2812 RGB LED matrix (GPIO 14)
- **Audio**: 3W Class D amplifier with speaker (I2S GPIO 4, 5, 2)
- **Controls**: Play button (GPIO 33), Reset button (GPIO 34), Volume potentiometer (GPIO 6)

## Development Phases

### Phase 1 (Complete)
WiFi provisioning via BLE with LED matrix status indicators and scrolling text feedback.

### Phase 2 (Complete)
Audio streaming via I2S with MP3 playback from LittleFS and HTTPS downloads.

### Phase 3 (In Progress)
Story selection interface, additional button controls, and cloud service integration.

## Key Features
- WiFi provisioning using ESP BLE Provisioning app
- Visual feedback via 8x8 LED matrix with scrolling text
- Status indicators (orange for waiting, cyan for connecting, green for connected)
- MP3 audio playback with volume control and fade-in
- HTTPS audio streaming from Supabase Storage
- Play button for on-demand playback
- Reset button (hold 5s) to clear WiFi credentials
- Proof of Possession (PoP) security: `abcd1234`
- Device naming: `STORY_XXXXXX` (last 3 MAC address bytes)
