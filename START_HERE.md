# 🚀 START HERE

Welcome to your ESP32-S3 Story Device project!

## 📖 What is this?

A storytelling device for kids that:
- Connects to WiFi via Bluetooth provisioning
- Shows status on an 8x8 LED matrix
- Will stream audio stories (Phase 2)
- Will have button controls (Phase 3)

## ⚡ Quick Start (5 minutes)

1. **Install PlatformIO** (if not already installed)
   - VS Code: Install "PlatformIO IDE" extension
   - Or: `pip install platformio`

2. **Build and flash**
   ```bash
   pio run --target upload
   pio device monitor
   ```

3. **Provision via phone**
   - Install [ESP BLE Provisioning app](https://play.google.com/store/apps/details?id=com.espressif.provble)
   - Connect to device `STORY_XXXXXX`
   - Use PoP: `abcd1234`
   - Enter WiFi credentials

4. **Watch the LED matrix** show the connection status!

## 📚 Documentation

Choose your path:

### For Quick Testing
→ **[QUICKSTART.md](QUICKSTART.md)** - Get running in 5 minutes

### For Understanding the Project
→ **[README.md](README.md)** - Full project overview
→ **[ARCHITECTURE.md](ARCHITECTURE.md)** - System design

### For Development
→ **[PROJECT_STATUS.md](PROJECT_STATUS.md)** - Current status & roadmap
→ **[TODO.md](TODO.md)** - What's next
→ **[COMMANDS.md](COMMANDS.md)** - CLI reference

### For Testing
→ **[TESTING.md](TESTING.md)** - Comprehensive testing guide

### For Celebration 🎉
→ **[PHASE1_COMPLETE.md](PHASE1_COMPLETE.md)** - What we built!

## 🎯 Current Status

**Phase 1: COMPLETE** ✅
- WiFi provisioning via BLE
- LED matrix status indicators
- Auto-reconnect on boot

**Phase 2: NEXT** 📋
- Audio streaming via I2S
- Sample audio playback

**Phase 3: FUTURE** 📋
- Button controls
- Volume control
- Story selection

## 🔑 Key Files

```
src/
├── main.cpp              # Main application
├── led_matrix.cpp        # LED matrix control
└── wifi_provisioning.cpp # WiFi provisioning

include/
├── led_matrix.h          # LED matrix API
└── wifi_provisioning.h   # WiFi provisioning API

platformio.ini            # Build configuration
```

## 🆘 Need Help?

**Device not found?**
- Check Bluetooth is enabled
- Look for device name in serial output

**WiFi fails?**
- Use 2.4GHz WiFi (not 5GHz)
- Check password is correct

**LED matrix not working?**
- Check power supply (5V, >1A)
- Verify GPIO 14 connection

**Reset everything:**
```bash
pio run --target erase
pio run --target upload
```

## 🎊 What's Next?

1. **Test Phase 1** - Make sure WiFi provisioning works
2. **Plan Phase 2** - Research audio streaming options
3. **Build Phase 2** - Add audio playback
4. **Iterate** - Keep improving!

---

**Ready?** → Open **[QUICKSTART.md](QUICKSTART.md)** and let's go! 🚀
