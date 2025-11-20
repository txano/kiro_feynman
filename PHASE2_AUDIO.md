# Phase 2: Audio Streaming

## Current Status: In Progress 🚧

### Completed ✅

#### I2S Audio Driver
- **I2S peripheral**: I2S_NUM_0
- **Sample rate**: 44.1 kHz
- **Bit depth**: 16-bit
- **Channel format**: Mono (left channel only)
- **Communication format**: Standard I2S (Philips)

#### GPIO Configuration
| Signal | GPIO | Description |
|--------|------|-------------|
| BCLK   | 4    | Bit clock |
| LRCLK  | 5    | Left/Right word select |
| DOUT   | 2    | Data output to amplifier |

#### Hardware Setup
- **Amplifier**: MAX98357A (or compatible 3W Class D)
- **Speaker**: 4-8Ω, 3W rated
- **Power**: 3.3V or 5V to amplifier VIN
- **Common ground**: ESP32 and amplifier share GND

#### Test Tones
- **BLE Ready**: 800 Hz sine wave, 200ms duration
- **WiFi Connected**: 1200 Hz sine wave, 300ms duration
- **Volume**: Amplitude set to 10,000 (out of 32,767 max)

### Implementation Details

#### Audio Module API (`audio.h`)
```cpp
void audio_init();                              // Initialize I2S
void audio_play_tone(uint16_t freq, uint32_t ms); // Play tone
void audio_play_ble_ready_tone();               // 800 Hz
void audio_play_wifi_connected_tone();          // 1200 Hz
```

#### Tone Generation
- Generates sine wave samples in memory
- Uses `sin()` function from math.h
- Writes samples to I2S via DMA
- Blocking operation (waits for playback to complete)

#### Integration with Main App
- Audio initialized after LED matrix in `setup()`
- Tones triggered by WiFi provisioning status callbacks
- State flags prevent duplicate tone playback

### Next Steps 📋

#### 1. Audio Streaming Library
Research and select:
- **ESP32-audioI2S**: Popular, supports MP3/AAC/WAV
- **ESP32-A2DP**: Bluetooth audio (alternative)
- **Custom implementation**: More control, more work

#### 2. HTTP Audio Client
- Implement HTTP/HTTPS client for streaming
- Handle chunked transfer encoding
- Manage connection timeouts and retries
- Buffer incoming audio data

#### 3. Audio Decoding
- Add MP3 decoder (libhelix or similar)
- Or use WAV files (no decoding needed)
- Or use AAC decoder (more complex)

#### 4. Buffer Management
- Implement ring buffer for audio data
- Balance buffer size vs latency
- Handle buffer underruns gracefully
- Monitor buffer fill level

#### 5. Playback Controls
- Play/pause functionality
- Stop and reset
- Volume control (amplitude scaling)
- Track position tracking

#### 6. LED Matrix Integration
- Show "Playing" animation
- Display track name (scrolling)
- Show playback progress
- Visual volume indicator

### Technical Decisions Needed

#### Audio Format
- **MP3**: Most common, good compression, decoder available
- **AAC**: Better quality, more complex decoder
- **WAV**: No decoding needed, large file sizes
- **Opus**: Best for speech, less common

**Recommendation**: Start with WAV for simplicity, add MP3 later

#### Streaming vs Download
- **Streaming**: Lower latency, requires stable connection
- **Download**: More reliable, requires storage space
- **Hybrid**: Download while playing (progressive download)

**Recommendation**: Progressive download to SPIFFS

#### Buffer Size
- **Small (4-8 KB)**: Lower latency, more prone to underruns
- **Medium (16-32 KB)**: Good balance
- **Large (64+ KB)**: More stable, higher latency

**Recommendation**: 32 KB ring buffer

### Testing Plan

#### Phase 2A: WAV Playback (Current Target)
1. Generate test WAV file (16-bit, 44.1kHz, mono)
2. Host on local HTTP server
3. Implement HTTP client
4. Stream and play WAV file
5. Verify audio quality

#### Phase 2B: MP3 Playback
1. Add MP3 decoder library
2. Test with sample MP3 files
3. Optimize buffer management
4. Test with various bitrates

#### Phase 2C: Cloud Integration
1. Set up cloud storage for audio files
2. Implement HTTPS client
3. Add authentication
4. Test with production audio files

### Known Issues & Limitations

#### Current
- Tone generation is blocking (no other tasks during playback)
- No volume control yet
- No error handling for I2S failures
- Amplitude hardcoded (10,000)

#### Future Considerations
- I2S DMA buffer size may need tuning
- May need FreeRTOS task for audio playback
- WiFi and audio may compete for CPU time
- Need to test power consumption with audio

### Hardware Notes

#### Amplifier Connection
```
ESP32-S3          MAX98357A
GPIO 4  ────────► BCLK
GPIO 5  ────────► LRC
GPIO 2  ────────► DIN
3.3V    ────────► VIN
GND     ────────► GND
                  │
                  ├─► OUT+ ──► Speaker +
                  └─► OUT- ──► Speaker -
```

#### Optional Amplifier Pins
- **SD (Shutdown)**: Tie to VIN for always-on, or GPIO for control
- **GAIN**: GND=9dB, Float=12dB, VIN=15dB

#### Tested Configuration
- Amplifier: MAX98357A
- Speaker: 4Ω, 3W
- Power: 3.3V (works, but 5V recommended for louder volume)
- Gain: Floating (12dB)

### Code Changes Summary

#### New Files
- `include/audio.h` - Audio API declarations
- `src/audio.cpp` - I2S driver and tone generation

#### Modified Files
- `src/main.cpp` - Added audio initialization and tone triggers
- `README.md` - Updated GPIO pinout and features
- `PROJECT_STATUS.md` - Updated phase status
- `TODO.md` - Updated Phase 2 checklist
- `ARCHITECTURE.md` - Added audio component to diagrams

### Resources

- [ESP-IDF I2S Driver](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/i2s.html)
- [MAX98357A Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX98357A-MAX98357B.pdf)
- [ESP32-audioI2S Library](https://github.com/schreibfaul1/ESP32-audioI2S)
- [I2S Audio Tutorial](https://www.digikey.com/en/maker/projects/getting-started-with-i2s-audio-on-esp32/8f0d0d6b0e0e4f0e8f0d0d6b0e0e4f0e)

---

**Last Updated**: Phase 2 started - Basic I2S and tone generation working
**Branch**: `feature/audio-streaming`
**Next Milestone**: HTTP streaming and WAV playback
