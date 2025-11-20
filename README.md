# Story Device for Kids

An ESP32-S3 based storytelling device with LED matrix display and audio streaming capabilities.

## Hardware

- **Board**: ESP32-S3-Matrix (Waveshare)
- **LED Matrix**: 8x8 WS2812 RGB LEDs (GPIO 14)
- **Audio**: 3W Class D Amplifier + Speaker
- **Controls**: Buttons + Potentiometer (to be implemented)

## Features

### Phase 1 (Current) ✅
- [x] WiFi provisioning via SoftAP
- [x] LED matrix status indicators with scrolling text
- [x] Visual feedback during provisioning

### Phase 2 (Next)
- [ ] Audio streaming via I2S
- [ ] Sample audio playback

### Phase 3 (Future)
- [ ] Button controls
- [ ] Volume control via potentiometer
- [ ] Story selection interface
- [ ] Cloud service integration

## Setup

### Prerequisites

1. Install [PlatformIO](https://platformio.org/install)
2. Install [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/) (handled by PlatformIO)

### Building and Flashing

```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

## WiFi Provisioning

### Using ESP SoftAP Provisioning App

1. Download the app:
   - **Android**: [ESP SoftAP Provisioning](https://play.google.com/store/apps/details?id=com.espressif.provsoftap)
   - **iOS**: [ESP SoftAP Provisioning](https://apps.apple.com/app/esp-softap-provisioning/id1474040630)

2. Power on the device
3. LED matrix will scroll "No WiFi" in orange
4. Device creates a WiFi access point named `STORY_XXXXXX` (last 3 bytes of MAC address)
5. Connect your phone to the `STORY_XXXXXX` WiFi network
6. Open the ESP SoftAP Provisioning app
7. Enter Proof of Possession: `abcd1234`
8. Select your WiFi network and enter password
9. LED matrix will show:
   - "Connecting..." in cyan (attempting connection)
   - "IP: xxx.xxx.xxx.xxx" in green (successfully connected)
   - "Error!" or "Retrying..." in orange (if connection fails)

### LED Matrix Status Display

- **Orange "No WiFi"**: Waiting for provisioning
- **Cyan "Connecting..."**: Attempting to connect to WiFi
- **Green "IP: xxx.xxx.xxx.xxx"**: Successfully connected (scrolling IP address)
- **Orange "Retrying..."**: Connection failed, retrying
- **Orange "Error!"**: Provisioning error occurred

## Project Structure

```
├── include/
│   ├── led_matrix.h          # LED matrix control
│   └── wifi_provisioning.h   # WiFi provisioning
├── src/
│   ├── main.cpp              # Main application
│   ├── led_matrix.cpp        # LED matrix implementation
│   └── wifi_provisioning.cpp # WiFi provisioning implementation
├── platformio.ini            # PlatformIO configuration
├── partitions.csv            # Flash partition table
└── sdkconfig.defaults        # ESP-IDF default config
```

## GPIO Pinout

| Function | GPIO | Notes |
|----------|------|-------|
| LED Matrix | 14 | WS2812 data line |
| I2S BCLK | 33 | Audio bit clock (future) |
| I2S LRCLK | 34 | Audio L/R clock (future) |
| I2S DOUT | 35 | Audio data out (future) |
| Button 1 | 1 | Story select (future) |
| Button 2 | 2 | Play/Pause (future) |
| Button 3 | 3 | Next/Previous (future) |
| Potentiometer | 4 | Volume control ADC (future) |

## Troubleshooting

### Device not showing as WiFi access point
- Check serial monitor for device name (STORY_XXXXXX)
- Make sure device is not already provisioned (erase flash if needed)
- Try resetting the device

### Cannot connect to STORY_XXXXXX network
- The network is open (no password required)
- Make sure you're within range
- Check serial monitor for errors

### WiFi connection fails (Auth Error 202)
- Verify WiFi password is correct
- Check if WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Ensure WPA2 security (WPA3 not supported)

### Device shows "Getting IP..." but never gets IP
- This was a bug, now fixed - device gets IP from ESP-IDF directly
- If still occurring, check router DHCP settings

### LED matrix not working
- Verify GPIO 14 connection
- Check power supply (5V, sufficient current)
- Brightness is set to 10 to prevent overheating

### Reset provisioning
To clear saved WiFi credentials and start fresh:
```bash
pio run --target erase
pio run --target upload
```

Or temporarily enable the reset in code (already in main.cpp for testing)

## Next Steps

1. Test WiFi provisioning with your phone
2. Verify LED matrix status indicators
3. Move to Phase 2: Audio streaming implementation

## Resources

- [Waveshare ESP32-S3-Matrix Wiki](https://www.waveshare.com/wiki/ESP32-S3-Matrix)
- [ESP-IDF Provisioning Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/provisioning/provisioning.html)
- [Adafruit NeoMatrix Library](https://github.com/adafruit/Adafruit_NeoMatrix)
