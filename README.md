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
- [x] LED matrix status indicators
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

### Using ESP BLE Provisioning App

1. Download the app:
   - **Android**: [ESP BLE Provisioning](https://play.google.com/store/apps/details?id=com.espressif.provble)
   - **iOS**: [ESP BLE Provisioning](https://apps.apple.com/app/esp-ble-provisioning/id1473590141)

2. Power on the device
3. LED matrix will show Bluetooth icon (blue)
4. Open the app and scan for devices
5. Look for device named `STORY_XXXXXX` (last 3 bytes of MAC address)
6. Enter Proof of Possession: `abcd1234`
7. Select your WiFi network and enter password
8. LED matrix will show:
   - Bluetooth icon → Checkmark (credentials received)
   - WiFi icon (connecting)
   - WiFi icon → Checkmark (connected)
   - Final checkmark (ready)

### LED Matrix Status Indicators

- **Blue Bluetooth icon**: Waiting for BLE connection
- **Green WiFi icon**: Connecting to WiFi
- **Green Checkmark**: Successfully connected
- **Red X**: Error occurred

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
- Check serial monitor for device name
- Try resetting the device

### WiFi connection fails
- Verify WiFi credentials
- Check if WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Check signal strength

### LED matrix not working
- Verify GPIO 14 connection
- Check power supply (5V, sufficient current)
- Brightness is set to 10 to prevent overheating

### Reset provisioning
To clear saved WiFi credentials and start fresh, you can erase NVS:
```bash
pio run --target erase
```

## Next Steps

1. Test WiFi provisioning with your phone
2. Verify LED matrix status indicators
3. Move to Phase 2: Audio streaming implementation

## Resources

- [Waveshare ESP32-S3-Matrix Wiki](https://www.waveshare.com/wiki/ESP32-S3-Matrix)
- [ESP-IDF Provisioning Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/provisioning/provisioning.html)
- [Adafruit NeoMatrix Library](https://github.com/adafruit/Adafruit_NeoMatrix)
