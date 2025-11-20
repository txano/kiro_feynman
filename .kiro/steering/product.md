# Product Overview

Story Device for Kids - An ESP32-S3 based storytelling device with LED matrix display and audio streaming capabilities.

## Hardware Platform
- **Board**: Waveshare ESP32-S3-Matrix
- **Display**: 8x8 WS2812 RGB LED matrix (GPIO 14)
- **Audio**: 3W Class D amplifier with speaker (I2S interface)
- **Controls**: Buttons and potentiometer (planned)

## Development Phases

### Phase 1 (Complete)
WiFi provisioning via SoftAP with LED matrix status indicators and scrolling text feedback.

### Phase 2 (Next)
Audio streaming via I2S with sample audio playback.

### Phase 3 (Future)
Button controls, volume adjustment, story selection interface, and cloud service integration.

## Key Features
- WiFi provisioning using ESP SoftAP Provisioning app
- Visual feedback via 8x8 LED matrix with scrolling text
- Status indicators (orange for waiting, cyan for connecting, green for connected)
- Proof of Possession (PoP) security: `abcd1234`
- Device naming: `STORY_XXXXXX` (last 3 MAC address bytes)
