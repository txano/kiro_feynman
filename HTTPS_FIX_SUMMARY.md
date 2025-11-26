# HTTPS/TLS Fix for ESP32-S3 (No PSRAM)

## Problem Identified

Errors encountered:
1. `-0x7780` / `-0x7680` = `MBEDTLS_ERR_SSL_ALLOC_FAILED` - memory allocation failure
2. `-0x2700` = `MBEDTLS_ERR_X509_CERT_VERIFY_FAILED` - certificate verification failure

## Root Causes

1. **Memory Issue**: ESP32-S3 with no PSRAM (~300KB RAM) + default 16KB SSL buffers too small for Supabase
2. **Certificate Issue**: Arduino certificate bundle fails to attach properly
3. **Config Conflicts**: `sdkconfig.esp32s3` had insecure TLS settings enabled

## Changes Made

### 1. sdkconfig.defaults
- Increased `CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN` from 16KB to 18KB
- Reduced `CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN` to 4KB (we don't send much)
- Enabled dynamic buffer management to free memory after handshake
- Added asymmetric content length support

### 2. platformio.ini
- Added build flags to ensure mbedTLS settings are applied
- These override any Arduino framework defaults

### 3. src/audio.cpp
- Reduced HTTP client buffer from 4KB to 2KB
- Added TX buffer limit (1KB)
- Changed download buffer from 1KB to 512 bytes
- Added heap monitoring logs to track memory usage
- Disabled HTTP keep-alive to save memory
- Added small delays during download to prevent watchdog issues

## Next Steps

1. **Clean build required**:
   ```bash
   pio run --target clean
   pio run --target upload
   pio device monitor
   ```

2. **Monitor the logs** - you'll now see:
   - Free heap before HTTPS request
   - Largest free memory block
   - Heap status during download
   - Any allocation failures

3. **If still failing**, try:
   - Reduce `CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN` to 17KB
   - Use HTTP instead of HTTPS (change URL to `http://`)
   - Contact Supabase to check their SSL record sizes

## Expected Behavior

With these changes, you should see:
- Successful SSL handshake
- Download progress logs every 10KB
- Heap monitoring showing available memory
- MP3 file saved to LittleFS and played

## Memory Budget

- SSL buffers: ~22KB (18KB in + 4KB out)
- HTTP client: ~3KB
- Download buffer: 512 bytes
- Total HTTPS overhead: ~26KB
- Should work with 100KB+ free heap
