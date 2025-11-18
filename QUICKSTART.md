# Quick Start Guide

## 🚀 Get Started in 5 Minutes

### Prerequisites

1. **Install PlatformIO**
   - VS Code: Install "PlatformIO IDE" extension
   - Or CLI: `pip install platformio`

2. **Install ESP BLE Provisioning App** on your phone
   - [Android](https://play.google.com/store/apps/details?id=com.espressif.provble)
   - [iOS](https://apps.apple.com/app/esp-ble-provisioning/id1473590141)

### Step 1: Build and Flash (2 minutes)

```bash
# Connect your ESP32-S3 via USB
# Then run:
pio run --target upload
```

Wait for compilation and upload to complete.

### Step 2: Monitor Serial Output (30 seconds)

```bash
pio device monitor
```

You should see:
```
I (xxx) main: Story Device - Starting...
I (xxx) wifi_prov: Provisioning started with service name: STORY_A1B2C3
I (xxx) wifi_prov: Proof of Possession (PoP): abcd1234
```

**Note the device name** (e.g., `STORY_A1B2C3`)

### Step 3: Watch the LED Matrix (10 seconds)

The 8x8 LED matrix should show:
- Brief "INIT" text
- Blue Bluetooth icon (blinking/static)

### Step 4: Provision via Phone App (2 minutes)

1. Open **ESP BLE Provisioning** app
2. Tap **"Provision New Device"**
3. Select **STORY_XXXXXX** from the list
4. Enter PoP: **abcd1234**
5. Select your **WiFi network**
6. Enter **WiFi password**
7. Tap **"Provision"**

### Step 5: Watch the Magic! (30 seconds)

**LED Matrix will show:**
1. 🔵 Blue Bluetooth icon → ✅ Checkmark (credentials received)
2. 🟢 Green WiFi icon (connecting)
3. 🟢 Green WiFi icon → ✅ Checkmark (connected)
4. ✅ Final checkmark (ready!)

**Serial Monitor will show:**
```
I (xxx) wifi_prov: Received Wi-Fi credentials - SSID:YourWiFi
I (xxx) wifi_prov: Connected with IP Address:192.168.x.x
I (xxx) main: WiFi Status: WIFI_CONNECTED
```

### ✅ Success!

Your device is now:
- Connected to WiFi
- Credentials saved (will auto-connect on next boot)
- Ready for Phase 2 (audio streaming)

---

## 🔧 Troubleshooting

### Device not found in app?
- Check Bluetooth is enabled on phone
- Device is within 5 meters
- Look for exact device name in serial output

### WiFi connection fails?
- Verify WiFi is 2.4GHz (not 5GHz)
- Check password is correct
- Ensure good signal strength

### LED matrix not working?
- Check power supply (5V, >1A)
- Verify it's the Waveshare ESP32-S3-Matrix board
- Check serial output for errors

### Need to reset?
```bash
pio run --target erase
pio run --target upload
```

---

## 📚 Next Steps

1. ✅ **Phase 1 Complete** - WiFi provisioning works!
2. 📋 **Phase 2 Next** - Add audio streaming
3. 📋 **Phase 3 Later** - Add buttons and UI
4. 📋 **Phase 4 Future** - Cloud service integration

See **PROJECT_STATUS.md** for detailed roadmap.

---

## 🆘 Need Help?

- **Full docs**: See README.md
- **Testing guide**: See TESTING.md
- **Commands**: See COMMANDS.md
- **Architecture**: See ARCHITECTURE.md
