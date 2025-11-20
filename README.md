# Story Device for Kids

An ESP32-S3 based storytelling device with LED matrix display and audio streaming capabilities.

## Hardware

- **Board**: ESP32-S3-Matrix (Waveshare)
- **LED Matrix**: 8x8 WS2812 RGB LEDs (GPIO 14)
- **Audio**: 3W Class D Amplifier + Speaker
- **Controls**: Buttons + Potentiometer (to be implemented)

## Features

### Phase 1 (Current) ✅
- [x] WiFi provisioning via BLE
- [x] LED matrix status indicators with scrolling text
- [x] Visual feedback during provisioning
- [x] Physical button reset (hold BOOT for 5 seconds)
- [x] Credentials persist across power cycles

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

### Using ESP BLE Provisioning App

1. Download the app:
   - **Android**: [ESP BLE Provisioning](https://play.google.com/store/apps/details?id=com.espressif.provble)
   - **iOS**: [ESP BLE Provisioning](https://apps.apple.com/app/esp-ble-provisioning/id1473590141)

2. Power on the device
3. LED matrix will scroll "BLE Ready" in orange
4. Open the ESP BLE Provisioning app on your phone
5. Enable Bluetooth on your phone
6. Tap "Provision New Device"
7. Select `STORY_XXXXXX` from the list (last 3 bytes of MAC address)
8. Enter Proof of Possession: `abcd1234`
9. Select your WiFi network and enter password
10. LED matrix will show:
    - "Connecting..." in cyan (attempting connection)
    - "IP: xxx.xxx.xxx.xxx" in green (successfully connected)
    - "Retrying..." in orange (if connection fails)

### LED Matrix Status Display

- **Orange "BLE Ready"**: Waiting for BLE provisioning
- **Cyan "Connecting..."**: Attempting to connect to WiFi
- **Green "IP: xxx.xxx.xxx.xxx"**: Successfully connected (scrolling IP address)
- **Orange "Retrying..."**: Connection failed, retrying

### Reset WiFi Credentials

Hold the **BOOT button** for 5 seconds to reset WiFi credentials. The device will show "RESET!" and restart. See [RESET_BUTTON_GUIDE.md](RESET_BUTTON_GUIDE.md) for details.

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

### Device not showing in BLE app
- Make sure Bluetooth is enabled on your phone
- Check serial monitor for device name (STORY_XXXXXX)
- Device should be within 10 meters
- Make sure device is not already provisioned

### WiFi connection fails (Auth Error 202)
- Verify WiFi password is correct
- Check if WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Ensure WPA2 security (WPA3 not supported)

### Credentials don't persist after power cycle
- This was a bug, now fixed
- Credentials are stored in NVS and persist across reboots

### LED matrix not working
- Verify GPIO 14 connection
- Check power supply (5V, sufficient current)
- Brightness is set to 10 to prevent overheating

### Reset provisioning
**Method 1: Physical Button (Recommended)**
- Hold BOOT button for 5 seconds
- Device will show "RESET!" and restart
- See [RESET_BUTTON_GUIDE.md](RESET_BUTTON_GUIDE.md)

**Method 2: Erase Flash**
```bash
pio run --target erase
pio run --target upload
```

## Next Steps

1. Test WiFi provisioning with your phone
2. Verify LED matrix status indicators
3. Move to Phase 2: Audio streaming implementation

## Resources

- [Waveshare ESP32-S3-Matrix Wiki](https://www.waveshare.com/wiki/ESP32-S3-Matrix)
- [ESP-IDF Provisioning Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/provisioning/provisioning.html)
- [Adafruit NeoMatrix Library](https://github.com/adafruit/Adafruit_NeoMatrix)
