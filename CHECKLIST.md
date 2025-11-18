# Pre-Flight Checklist ✈️

Before testing on hardware, verify everything is ready:

## 📦 Project Files

- [x] platformio.ini - Build configuration
- [x] partitions.csv - Flash partition table
- [x] sdkconfig.defaults - ESP-IDF configuration
- [x] .gitignore - Git ignore rules

## 💻 Source Code

- [x] src/main.cpp - Main application
- [x] src/led_matrix.cpp - LED matrix implementation
- [x] src/wifi_provisioning.cpp - WiFi provisioning implementation
- [x] include/led_matrix.h - LED matrix header
- [x] include/wifi_provisioning.h - WiFi provisioning header

## 📚 Documentation

- [x] START_HERE.md - Entry point
- [x] README.md - Project overview
- [x] QUICKSTART.md - 5-minute guide
- [x] TESTING.md - Testing procedures
- [x] COMMANDS.md - CLI reference
- [x] ARCHITECTURE.md - System design
- [x] PROJECT_STATUS.md - Status & roadmap
- [x] TODO.md - Future work
- [x] PHASE1_COMPLETE.md - Phase 1 summary
- [x] CHECKLIST.md - This file

## 🔧 Prerequisites

Before building, ensure you have:

- [ ] PlatformIO installed
  - VS Code extension OR
  - CLI via pip
- [ ] USB cable for ESP32-S3
- [ ] ESP32-S3-Matrix board
- [ ] ESP BLE Provisioning app on phone
  - [Android](https://play.google.com/store/apps/details?id=com.espressif.provble)
  - [iOS](https://apps.apple.com/app/esp-ble-provisioning/id1473590141)

## 🔌 Hardware Check

- [ ] ESP32-S3-Matrix board
- [ ] 8x8 LED matrix (built-in, GPIO 14)
- [ ] USB-C cable
- [ ] Power supply (5V, >1A)
- [ ] Computer with USB port

## 🏗️ Build Test

Run these commands to verify everything compiles:

```bash
# Clean build
pio run --target clean

# Build project
pio run
```

Expected output:
```
Building .pio/build/esp32-s3-devkitc-1/firmware.bin
...
SUCCESS
```

## 📱 Phone Setup

- [ ] Bluetooth enabled on phone
- [ ] ESP BLE Provisioning app installed
- [ ] WiFi credentials ready (2.4GHz network)
- [ ] Phone close to device (<5m)

## 🧪 First Test Plan

1. **Build & Flash**
   ```bash
   pio run --target upload
   ```
   - [ ] Compilation succeeds
   - [ ] Upload succeeds
   - [ ] No errors

2. **Monitor Serial**
   ```bash
   pio device monitor
   ```
   - [ ] Device boots
   - [ ] Shows "Story Device - Starting..."
   - [ ] Shows device name (STORY_XXXXXX)
   - [ ] Shows PoP: abcd1234

3. **Check LED Matrix**
   - [ ] Shows "INIT" text briefly
   - [ ] Shows blue Bluetooth icon
   - [ ] LEDs are visible (not too dim)
   - [ ] LEDs are not too bright (no overheating)

4. **Provision via App**
   - [ ] Device appears in app
   - [ ] Can connect with PoP
   - [ ] Can select WiFi network
   - [ ] Can enter password
   - [ ] Provisioning succeeds

5. **Verify Connection**
   - [ ] LED shows WiFi icon (green)
   - [ ] LED shows checkmark
   - [ ] Serial shows IP address
   - [ ] No errors in serial

6. **Test Auto-Reconnect**
   - [ ] Reset device (press reset button)
   - [ ] Device boots
   - [ ] Automatically connects to WiFi
   - [ ] No need to provision again
   - [ ] LED shows correct status

## 🐛 Troubleshooting Checklist

If something doesn't work:

### Build Fails
- [ ] PlatformIO is up to date
- [ ] Internet connection (for downloading dependencies)
- [ ] Disk space available
- [ ] Check error message in output

### Upload Fails
- [ ] USB cable is connected
- [ ] Correct COM port selected
- [ ] Device is in bootloader mode (may need to hold BOOT button)
- [ ] No other program using the port

### LED Matrix Not Working
- [ ] Power supply is adequate
- [ ] GPIO 14 connection is correct
- [ ] Board is ESP32-S3-Matrix (not generic ESP32)
- [ ] Check serial for initialization messages

### BLE Not Working
- [ ] Bluetooth enabled on phone
- [ ] Phone is close to device
- [ ] No other BLE provisioning in progress
- [ ] Check serial for "Provisioning started" message

### WiFi Connection Fails
- [ ] WiFi is 2.4GHz (not 5GHz)
- [ ] Password is correct
- [ ] Signal strength is good
- [ ] Router allows new devices

## ✅ Success Criteria

Phase 1 is successful when:

- [x] Code compiles without errors
- [ ] Device boots successfully
- [ ] LED matrix shows status
- [ ] BLE provisioning works
- [ ] Device connects to WiFi
- [ ] Device auto-reconnects on reboot
- [ ] Serial output is clean
- [ ] No crashes or resets

## 🎯 Ready for Phase 2?

Before moving to Phase 2 (audio streaming), ensure:

- [ ] All Phase 1 tests pass
- [ ] Device is stable (no random resets)
- [ ] WiFi connection is reliable
- [ ] LED matrix works consistently
- [ ] You understand the code structure
- [ ] Documentation is clear

## 📝 Notes

Use this space to note any issues or observations:

```
Date: ___________

Issues found:
-
-
-

Observations:
-
-
-

Next steps:
-
-
-
```

---

**Once all checkboxes are complete, you're ready to move to Phase 2!** 🎉
