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
   - Then show orange "BLE Ready" (scrolling)

## Provisioning Test

### Method 1: ESP BLE Provisioning App (Recommended)

1. Install app on your phone
2. Enable Bluetooth on your phone
3. Open ESP BLE Provisioning app
4. Tap "Provision New Device"
5. Select `STORY_XXXXXX` from the list
6. Enter PoP: `abcd1234`
7. Select your WiFi network
8. Enter WiFi password
9. Wait for connection

**Expected LED sequence**:
- Orange "BLE Ready" (scrolling)
- Cyan "Connecting..." (scrolling)
- Green "IP: 192.168.x.x" (scrolling)

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
I (xxx) wifi_prov: Generated service name: STORY_XXXXXX
I (xxx) wifi_prov: ✓ BLE device name: STORY_XXXXXX
I (xxx) wifi_prov: ✓ Use ESP BLE Provisioning app to connect
I (xxx) wifi_prov: ✓ Enter PoP: abcd1234 when prompted
I (xxx) main: WiFi Status: AP_STARTED
```

After connecting via app:
```
I (xxx) wifi_prov: WIFI_PROV_CRED_RECV - Received Wi-Fi credentials - SSID:YourWiFi
I (xxx) main: WiFi Status: WIFI_CONNECTING
I (xxx) wifi_prov: WIFI_PROV_CRED_SUCCESS - Provisioning successful, waiting for IP...
I (xxx) wifi_prov: IP_EVENT_STA_GOT_IP - Connected with IP Address:192.168.x.x
I (xxx) main: WiFi Status: WIFI_CONNECTED
I (xxx) main: Checking IP: 192.168.x.x (wifi_connected=1)
I (xxx) main: IP Address updated: IP: 192.168.x.x
```

## Troubleshooting

### Issue: Device not found in BLE app
**Check**:
- Serial output shows "Provisioning started"
- Device is not already provisioned
- Bluetooth is enabled on phone
- Phone is close to device (<10m)

**Fix**:
- Reset device (press reset button or hold BOOT for 5 seconds)
- Erase flash: `pio run --target erase`
- Check serial for actual device name
- Restart Bluetooth on phone

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

### Issue: LED matrix shows "Error!" or "Retrying..."
**Meaning**: WiFi connection failed (wrong password or network issue)

**Fix**:
- Check serial output for disconnect reason (202 = auth error, 201 = AP not found)
- Verify WiFi password is correct
- Ensure WiFi is 2.4GHz and WPA2
- Check WiFi signal strength
- Reset provisioning and try again

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
- [x] Device connects to WiFi on first boot
- [x] Device auto-connects on subsequent boots
- [x] LED matrix shows correct status at each step
- [x] Serial output is clean and informative
- [x] Credentials persist across power cycles
- [x] Physical button reset works (BOOT button for 5 seconds)

✅ **Phase 1 Complete!** Ready to move to **Phase 2: Audio Streaming**!
