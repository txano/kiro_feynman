# Quick Reference Card

## Device Information

**Board**: ESP32-S3-Matrix (Waveshare)  
**Device Name**: `STORY_XXXXXX` (last 3 bytes of MAC)  
**Proof of Possession**: `abcd1234`

## LED Matrix Status

| Display | Color | Meaning |
|---------|-------|---------|
| "INIT" | White | Device starting up |
| "No WiFi" | 🟠 Orange | Waiting for provisioning |
| "Connecting..." | 🔵 Cyan | Attempting WiFi connection |
| "IP: xxx.xxx.xxx.xxx" | 🟢 Green | Connected successfully |
| "Retrying..." | 🟠 Orange | Connection failed, retrying |
| "Error!" | 🟠 Orange | Provisioning error |

## Provisioning Steps

1. **Power on device** → LED shows "No WiFi"
2. **Connect phone** to `STORY_XXXXXX` WiFi (open, no password)
3. **Open app**: ESP SoftAP Provisioning
4. **Enter PoP**: `abcd1234`
5. **Select WiFi** and enter password
6. **Wait** → LED shows "Connecting..." then "IP: xxx.xxx.xxx.xxx"

## App Download

**Android**: [ESP SoftAP Provisioning](https://play.google.com/store/apps/details?id=com.espressif.provsoftap)  
**iOS**: [ESP SoftAP Provisioning](https://apps.apple.com/app/esp-softap-provisioning/id1474040630)

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

### Cannot find STORY_XXXXXX network
- Check serial monitor for actual AP name
- Device might already be provisioned (erase flash)
- Reset device

### WiFi connection fails (Error 202)
- Wrong password
- Check WiFi is 2.4GHz (not 5GHz)
- Ensure WPA2 security

### LED shows "Getting IP..." forever
- This was a bug, now fixed
- Update to latest code

### Need to re-provision
```bash
pio run --target erase
pio run --target upload
```

Or enable `wifi_prov_reset()` in code temporarily

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
I (xxx) wifi_prov: ✓ AP name: STORY_A1B2C3
I (xxx) wifi_prov: ✓ Provisioning started successfully!
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
⚠️ **Testing Mode**: `wifi_prov_reset()` is enabled in code - remove for production

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

**Last Updated**: Phase 1 Complete  
**Version**: 1.0  
**Status**: Production Ready (after removing test reset)
