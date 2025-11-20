# Quick Reference Card

## Device Information

**Board**: ESP32-S3-Matrix (Waveshare)  
**Device Name**: `STORY_XXXXXX` (last 3 bytes of MAC)  
**Proof of Possession**: `abcd1234`

## LED Matrix Status

| Display | Color | Meaning |
|---------|-------|---------|
| "INIT" | White | Device starting up |
| "BLE Ready" | 🟠 Orange | Waiting for BLE provisioning |
| "Connecting..." | 🔵 Cyan | Attempting WiFi connection |
| "IP: xxx.xxx.xxx.xxx" | 🟢 Green | Connected successfully |
| "Retrying..." | 🟠 Orange | Connection failed, retrying |
| "RESET!" | 🟠 Orange | Resetting WiFi credentials |

## Provisioning Steps

1. **Power on device** → LED shows "BLE Ready"
2. **Enable Bluetooth** on phone
3. **Open app**: ESP BLE Provisioning
4. **Select device**: `STORY_XXXXXX`
5. **Enter PoP**: `abcd1234`
6. **Select WiFi** and enter password
7. **Wait** → LED shows "Connecting..." then "IP: xxx.xxx.xxx.xxx"

## App Download

**Android**: [ESP BLE Provisioning](https://play.google.com/store/apps/details?id=com.espressif.provble)  
**iOS**: [ESP BLE Provisioning](https://apps.apple.com/app/esp-ble-provisioning/id1473590141)

## Common Commands

```bash
# Build and upload
pio run --target upload

# Monitor serial output
pio device monitor

# Erase flash (reset WiFi credentials)
pio run --target erase

# Full rebuild
pio run --target clean && pio run --target upload
```

## Troubleshooting

### Device not found in BLE app
- Enable Bluetooth on phone
- Check serial monitor for actual device name
- Device should be within 10 meters
- Device might already be provisioned

### WiFi connection fails (Error 202)
- Wrong password
- Check WiFi is 2.4GHz (not 5GHz)
- Ensure WPA2 security

### Credentials don't persist
- This was a bug, now fixed
- Credentials stored in NVS persist across reboots

### Need to re-provision

**Method 1: Physical Button (Recommended)**
- Hold BOOT button for 5 seconds
- Device shows "RESET!" and restarts

**Method 2: Erase Flash**
```bash
pio run --target erase
pio run --target upload
```

## GPIO Pinout

| Function | GPIO | Status |
|----------|------|--------|
| LED Matrix | 14 | ✅ Active |
| I2S BCLK | 33 | Phase 2 |
| I2S LRCLK | 34 | Phase 2 |
| I2S DOUT | 35 | Phase 2 |
| Button 1 | 1 | Phase 3 |
| Button 2 | 2 | Phase 3 |
| Button 3 | 3 | Phase 3 |
| Potentiometer | 4 | Phase 3 |

## Serial Monitor Output (Normal)

```
I (xxx) main: Story Device - Starting...
I (xxx) main: Initializing LED matrix...
I (xxx) main: Initializing WiFi provisioning...
I (xxx) wifi_prov: Generated service name: STORY_A1B2C3
I (xxx) wifi_prov: ✓ BLE device name: STORY_A1B2C3
I (xxx) wifi_prov: ✓ Use ESP BLE Provisioning app to connect
I (xxx) main: WiFi Status: AP_STARTED
```

After provisioning:
```
I (xxx) wifi_prov: WIFI_PROV_CRED_RECV - Received Wi-Fi credentials
I (xxx) main: WiFi Status: WIFI_CONNECTING
I (xxx) wifi_prov: IP_EVENT_STA_GOT_IP - Connected with IP Address:192.168.x.x
I (xxx) main: WiFi Status: WIFI_CONNECTED
I (xxx) main: IP Address updated: IP: 192.168.x.x
```

## Important Notes

⚠️ **LED Brightness**: Set to 10 (max 255) to prevent overheating  
⚠️ **WiFi**: Only 2.4GHz supported (ESP32 limitation)  
⚠️ **Power**: Requires 5V, >1A supply  
⚠️ **Reset**: Hold BOOT button for 5 seconds to reset WiFi credentials  
✅ **Credentials**: Persist across power cycles in NVS

## File Locations

**Source Code**:
- `src/main.cpp` - Main application
- `src/wifi_provisioning.cpp` - WiFi provisioning
- `src/led_matrix.cpp` - LED matrix control

**Headers**:
- `include/wifi_provisioning.h`
- `include/led_matrix.h`

**Configuration**:
- `platformio.ini` - Build config
- `partitions.csv` - Flash layout
- `sdkconfig.defaults` - ESP-IDF config

**Documentation**:
- `README.md` - Overview
- `QUICKSTART.md` - 5-minute guide
- `TESTING.md` - Testing procedures
- `COMMANDS.md` - Command reference
- `ARCHITECTURE.md` - System design
- `PROJECT_STATUS.md` - Current status
- `PHASE1_COMPLETE.md` - Phase 1 summary
- `PHASE1_CHANGES.md` - Changes from original plan

## Phase Status

- ✅ **Phase 1**: WiFi Provisioning + LED Matrix - COMPLETE
- 📋 **Phase 2**: Audio Streaming - NEXT
- 📋 **Phase 3**: User Interface (Buttons + Volume)
- 📋 **Phase 4**: Cloud Integration

---

**Last Updated**: Phase 1 Complete with BLE  
**Version**: 1.0  
**Status**: Production Ready
