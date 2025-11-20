# WiFi Credential Reset Guide

## Overview

The device now remembers WiFi credentials across reboots. To reset credentials, use the physical BOOT button on the ESP32-S3 board.

## How to Reset WiFi Credentials

### Method: Hold BOOT Button

1. **Locate the BOOT button** on your ESP32-S3-Matrix board
2. **Press and hold** the BOOT button
3. **Keep holding** for 5 seconds
4. **Watch the serial monitor** for countdown messages
5. **LED matrix will show** "RESET!" when credentials are cleared
6. **Device will restart** automatically
7. **BLE provisioning** will start fresh

### What Happens

**During button hold:**
```
I (xxx) main: Reset button pressed - hold for 5 seconds to reset WiFi
I (xxx) main: Hold for 4 more seconds...
I (xxx) main: Hold for 3 more seconds...
I (xxx) main: Hold for 2 more seconds...
I (xxx) main: Hold for 1 more seconds...
W (xxx) main: !!! RESETTING WiFi credentials !!!
I (xxx) main: WiFi credentials cleared. Restarting...
```

**LED Matrix shows:**
1. "RESET!" (when credentials cleared)
2. "RESTART" (before reboot)
3. "INIT" (after reboot)
4. "BLE Ready" (ready for provisioning)

### If You Release Too Early

If you release the button before 5 seconds:
```
I (xxx) main: Reset button released (held for 2345 ms - not long enough)
```

Nothing happens - credentials are safe!

## Normal Operation

### First Boot (No Credentials)
1. Device starts
2. LED shows "BLE Ready"
3. Use ESP BLE Provisioning app to configure WiFi
4. Device connects and shows IP address
5. Credentials saved to NVS (non-volatile storage)

### Subsequent Boots (Has Credentials)
1. Device starts
2. LED shows "Connecting..."
3. Device auto-connects to saved WiFi
4. LED shows "IP: xxx.xxx.xxx.xxx"
5. No provisioning needed!

## Technical Details

**Button Pin**: GPIO 0 (BOOT button)
**Hold Time**: 5000 milliseconds (5 seconds)
**Button Logic**: Active LOW (pressed = LOW, released = HIGH)
**Debouncing**: Built-in with hold time requirement

## Troubleshooting

### Button doesn't work
- Make sure you're holding the BOOT button (not RESET)
- Hold for full 5 seconds
- Check serial monitor for messages

### Device keeps resetting credentials
- Don't hold BOOT button during normal boot
- Check if button is stuck or shorted

### Want to change hold time
Edit in `src/main.cpp`:
```cpp
#define RESET_HOLD_TIME 5000  // Change to desired milliseconds
```

## Safety Features

1. **Long hold required**: Prevents accidental resets
2. **Visual feedback**: LED shows "RESET!" before clearing
3. **Serial logging**: All actions logged for debugging
4. **Automatic restart**: Device restarts cleanly after reset

## Use Cases

### Development/Testing
- Hold BOOT button to quickly reset and test provisioning again

### Production/Deployment
- User can reset device if they change WiFi network
- No need to reflash firmware to clear credentials

### Troubleshooting
- If device can't connect to WiFi (wrong password, network changed)
- Hold BOOT button to reset and re-provision

---

**Button**: BOOT (GPIO 0)
**Hold Time**: 5 seconds
**Action**: Clears WiFi credentials and restarts
**Safe**: Requires intentional long press
