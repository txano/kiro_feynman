# Testing Guide - Phase 1

## Quick Start

1. **Build and flash**:
   ```bash
   pio run --target upload
   pio device monitor
   ```

2. **Watch the serial output** for:
   - Device initialization
   - Service name (e.g., `STORY_A1B2C3`)
   - Proof of Possession: `abcd1234`

3. **Watch the LED matrix**:
   - Should show "INIT" text briefly
   - Then show blue Bluetooth icon (waiting for connection)

## Provisioning Test

### Method 1: ESP BLE Provisioning App (Recommended)

1. Install app on your phone
2. Open app and tap "Provision New Device"
3. Select `STORY_XXXXXX` from the list
4. Enter PoP: `abcd1234`
5. Select your WiFi network
6. Enter WiFi password
7. Wait for connection

**Expected LED sequence**:
- Blue BT icon (searching)
- Blue BT icon + checkmark (connected)
- Green WiFi icon (connecting)
- Green WiFi icon + checkmark (connected)
- Green checkmark (ready)

### Method 2: Command Line (Advanced)

Using `esp-idf/tools/esp_prov`:

```bash
python esp_prov.py --transport ble --service_name STORY_XXXXXX --pop abcd1234 --ssid "YourWiFi" --passphrase "YourPassword"
```

## Expected Serial Output

```
I (xxx) main: =================================
I (xxx) main: Story Device - Starting...
I (xxx) main: =================================
I (xxx) main: Initializing LED matrix...
I (xxx) main: Initializing WiFi provisioning...
I (xxx) wifi_prov: WiFi provisioning initialized
I (xxx) main: Starting WiFi provisioning...
I (xxx) wifi_prov: Starting provisioning
I (xxx) wifi_prov: Provisioning started with service name: STORY_XXXXXX
I (xxx) wifi_prov: Scan QR code or use ESP BLE Prov app
I (xxx) wifi_prov: Proof of Possession (PoP): abcd1234
I (xxx) main: WiFi Status: BLE_SEARCHING
```

After connecting via app:
```
I (xxx) wifi_prov: Received Wi-Fi credentials - SSID:YourWiFi
I (xxx) main: WiFi Status: BLE_CONNECTED
I (xxx) wifi_prov: Provisioning successful
I (xxx) main: WiFi Status: WIFI_CONNECTING
I (xxx) wifi_prov: Connected with IP Address:192.168.x.x
I (xxx) main: WiFi Status: WIFI_CONNECTED
```

## Troubleshooting

### Issue: Device not found in BLE app
**Check**:
- Serial output shows "Provisioning started"
- Bluetooth is enabled on phone
- Phone is close to device (<5m)
- No other BLE provisioning in progress

**Fix**:
- Reset device (press reset button)
- Restart BLE app
- Check serial for actual service name

### Issue: "Invalid PoP" error
**Check**:
- You entered exactly: `abcd1234`
- No extra spaces

### Issue: WiFi connection fails
**Check**:
- WiFi is 2.4GHz (not 5GHz)
- Password is correct
- Signal strength is good

**Serial will show**:
```
E (xxx) wifi_prov: Provisioning failed! Reason : Wi-Fi auth error
```

**Fix**:
- Reset provisioning: `pio run --target erase`
- Try again with correct credentials

### Issue: LED matrix shows red X
**Meaning**: Provisioning or WiFi connection failed

**Fix**:
- Check serial output for specific error
- Reset device and try again
- Verify WiFi credentials

### Issue: LED matrix not lighting up
**Check**:
- Power supply is adequate (5V, >1A)
- GPIO 14 connection
- Serial shows "Initializing LED matrix..."

**Fix**:
- Check hardware connections
- Verify board is ESP32-S3-Matrix
- Try increasing brightness in code (currently 10)

## Reset Provisioning

To test provisioning again after successful connection:

```bash
# Erase all flash (including NVS where WiFi creds are stored)
pio run --target erase

# Re-flash
pio run --target upload
```

Or add a reset button in code (future enhancement).

## Next Phase Preparation

Once WiFi provisioning works reliably:
- [ ] Device connects to WiFi on first boot
- [ ] Device auto-connects on subsequent boots
- [ ] LED matrix shows correct status at each step
- [ ] Serial output is clean and informative

Ready to move to **Phase 2: Audio Streaming**!
