# Quick Command Reference

## Build & Flash

```bash
# Build only
pio run

# Build and upload
pio run --target upload

# Upload and monitor
pio run --target upload && pio device monitor

# Monitor only (after upload)
pio device monitor

# Clean build
pio run --target clean

# Full clean (including dependencies)
pio run --target fullclean
```

## Flash Management

```bash
# Erase entire flash (removes WiFi credentials)
pio run --target erase

# Upload filesystem (if using SPIFFS in future)
pio run --target uploadfs
```

## Debugging

```bash
# Monitor with filter (show only errors/warnings)
pio device monitor --filter esp32_exception_decoder

# Monitor with baud rate
pio device monitor -b 115200

# List connected devices
pio device list
```

## Project Management

```bash
# Update dependencies
pio pkg update

# Install specific library
pio pkg install -l "adafruit/Adafruit NeoMatrix"

# Show project info
pio project config
```

## Useful PlatformIO Commands

```bash
# Check for updates
pio upgrade

# Update platforms
pio platform update

# List installed platforms
pio platform list
```

## ESP-IDF Specific (if needed)

```bash
# Generate sdkconfig (interactive)
pio run --target menuconfig

# Show partition table
pio run --target partition-table
```

## Testing Workflow

```bash
# 1. Clean build and upload
pio run --target clean && pio run --target upload

# 2. Monitor output
pio device monitor

# 3. If need to reset provisioning
pio run --target erase && pio run --target upload

# 4. Monitor again
pio device monitor
```
