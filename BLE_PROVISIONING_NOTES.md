# BLE Provisioning Branch

## Branch: feature/ble-provisioning

This branch converts the WiFi provisioning from SoftAP to BLE (Bluetooth Low Energy).

## Changes Made

### 1. wifi_provisioning.cpp
- Changed include from `scheme_softap.h` to `scheme_ble.h`
- Changed scheme from `wifi_prov_scheme_softap` to `wifi_prov_scheme_ble`
- Removed AP interface creation (only STA needed for BLE)
- Added custom BLE service UUID
- Updated logging messages to reflect BLE instead of SoftAP

### 2. sdkconfig.defaults
- Enabled Bluetooth: `CONFIG_BT_ENABLED=y`
- Enabled Bluedroid stack: `CONFIG_BT_BLUEDROID_ENABLED=y`
- Enabled BLE: `CONFIG_BT_BLE_ENABLED=y`
- Set BLE-only mode: `CONFIG_BTDM_CTRL_MODE_BLE_ONLY=y`
- Removed SoftAP support configuration

### 3. main.cpp
- Changed initial status message from "No WiFi" to "BLE Ready"

## Testing Instructions

### Build and Upload
```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

### Expected Serial Output
```
I (xxx) main: Story Device - Starting...
I (xxx) wifi_prov: WiFi provisioning initialized (STA interface created)
I (xxx) wifi_prov: Generated service name: STORY_XXXXXX
I (xxx) wifi_prov: ✓ Provisioning started successfully!
I (xxx) wifi_prov: ✓ BLE device name: STORY_XXXXXX
I (xxx) wifi_prov: ✓ Use ESP BLE Provisioning app to connect
I (xxx) wifi_prov: ✓ Enter PoP: abcd1234 when prompted
```

### LED Matrix Display
- Should show "BLE Ready" in orange (scrolling)
- After provisioning: "Connecting..." in cyan
- After connected: "IP: xxx.xxx.xxx.xxx" in green

### Provisioning via App

**Download App**:
- Android: [ESP BLE Provisioning](https://play.google.com/store/apps/details?id=com.espressif.provble)
- iOS: [ESP BLE Provisioning](https://apps.apple.com/app/esp-ble-provisioning/id1473590141)

**Steps**:
1. Power on device
2. Open ESP BLE Provisioning app
3. Enable Bluetooth on phone
4. Tap "Provision New Device"
5. Select `STORY_XXXXXX` from the list
6. Enter PoP: `abcd1234`
7. Select your WiFi network
8. Enter WiFi password
9. Wait for connection

## Advantages of BLE over SoftAP

### Pros
1. **No network switching**: User doesn't need to switch WiFi networks
2. **Better UX**: Simpler flow - just open app and connect
3. **More professional**: BLE is standard for IoT device provisioning
4. **Lower power**: BLE uses less power than WiFi AP mode
5. **Faster discovery**: BLE scanning is faster than WiFi scanning

### Cons
1. **BLE compatibility**: Some older phones may have BLE issues
2. **Range**: BLE has shorter range than WiFi (~10m vs ~30m)
3. **Complexity**: BLE stack adds ~50KB to firmware size

## Potential Issues & Solutions

### Issue: BLE not starting
**Check**:
- Bluetooth is enabled in sdkconfig
- Sufficient RAM available
- No conflicts with other BLE services

**Solution**:
- Check serial output for BLE initialization errors
- Verify sdkconfig.defaults is being applied
- Try `pio run --target fullclean` and rebuild

### Issue: Device not appearing in app
**Check**:
- Bluetooth is enabled on phone
- Phone is within 5-10 meters
- No other BLE provisioning in progress
- Device name in serial output

**Solution**:
- Restart phone Bluetooth
- Reset device
- Check serial for actual device name

### Issue: Connection fails during provisioning
**Check**:
- PoP is correct: `abcd1234`
- WiFi credentials are correct
- WiFi is 2.4GHz

**Solution**:
- Verify PoP entry
- Check WiFi password
- Try different WiFi network

## Comparison: SoftAP vs BLE

| Feature | SoftAP | BLE |
|---------|--------|-----|
| User switches network | Yes | No |
| Range | ~30m | ~10m |
| Power consumption | Higher | Lower |
| Firmware size | Smaller | +50KB |
| Phone compatibility | Universal | Most modern phones |
| Setup complexity | Medium | Low |
| Professional feel | Good | Better |

## Next Steps

1. **Test on physical device**
   - Verify BLE advertising works
   - Test with ESP BLE Provisioning app
   - Confirm WiFi connection succeeds
   - Verify IP address displays correctly

2. **Compare with SoftAP**
   - Test reliability
   - Measure connection time
   - Check user experience
   - Verify range

3. **Decision**
   - If BLE works well → merge to main
   - If issues found → stay with SoftAP or fix issues
   - Document findings

## Rollback to SoftAP

If BLE doesn't work well, switch back to main branch:

```bash
git checkout main
```

The SoftAP implementation is safe in the main branch.

## Documentation Updates Needed (if BLE is chosen)

If BLE provisioning works well and we decide to use it:

- [ ] Update README.md
- [ ] Update QUICKSTART.md
- [ ] Update TESTING.md
- [ ] Update ARCHITECTURE.md
- [ ] Update PHASE1_COMPLETE.md
- [ ] Update QUICK_REFERENCE.md
- [ ] Create new PHASE1_CHANGES.md for BLE

---

**Branch Created**: feature/ble-provisioning
**Status**: Ready for testing
**Pushed to GitHub**: Yes
**Main branch**: Safe with SoftAP implementation
