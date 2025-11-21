# Audio Streaming Guide

## Current Implementation

### Working Features ✅
- **I2S Audio Output**: GPIO 4 (BCLK), GPIO 5 (LRCLK), GPIO 2 (DOUT)
- **Tone Generation**: Simple sine wave tones for feedback
- **HTTP WAV Streaming**: Stream WAV files from any HTTP URL

### Audio Formats

#### WAV (PCM) - Currently Supported ✅
- **Pros**: No decoding needed, simple implementation
- **Cons**: Large file size
- **Best for**: Testing, development, short sounds
- **Supported**: 16-bit PCM, mono or stereo, any sample rate

#### MP3 - Planned 🚧
- **Pros**: Small file size, good for streaming
- **Cons**: Requires decoder library
- **Best for**: Production, long audio, speech
- **Status**: Library integration issues with PlatformIO

## AI Audio Service Integration

### ElevenLabs API
```python
# ElevenLabs returns MP3 by default
# Request WAV for testing:
response = requests.post(
    f"https://api.elevenlabs.io/v1/text-to-speech/{voice_id}",
    headers={"xi-api-key": API_KEY},
    json={
        "text": "Hello from ElevenLabs",
        "model_id": "eleven_monolingual_v1",
        "output_format": "pcm_16000"  # 16kHz PCM WAV
    }
)
```

### OpenAI TTS API
```python
# OpenAI TTS supports multiple formats
from openai import OpenAI
client = OpenAI()

response = client.audio.speech.create(
    model="tts-1",
    voice="alloy",
    input="Hello from OpenAI",
    response_format="wav"  # or "mp3", "opus", "aac", "flac"
)
```

## Testing WAV Streaming

### Option 1: Use a Public Test WAV File
```cpp
// In main.cpp, replace the tone with:
audio_play_wav_url("http://example.com/test.wav");
```

### Option 2: Host Your Own WAV File

**Using Python HTTP Server:**
```bash
# Create a test WAV file (or use ready.mp3 converted to WAV)
# In your project directory:
python -m http.server 8000

# Then use:
audio_play_wav_url("http://YOUR_COMPUTER_IP:8000/test.wav");
```

**Using ngrok (for internet access):**
```bash
# Start local server
python -m http.server 8000

# In another terminal:
ngrok http 8000

# Use the ngrok URL:
audio_play_wav_url("https://xxxx.ngrok.io/test.wav");
```

### Option 3: Convert ready.mp3 to WAV

**Using ffmpeg:**
```bash
ffmpeg -i ready.mp3 -acodec pcm_s16le -ar 44100 -ac 1 ready.wav
```

**Using online converter:**
- Upload ready.mp3 to https://cloudconvert.com/mp3-to-wav
- Settings: 44100 Hz, 16-bit, Mono
- Download ready.wav

## Code Example

### Play WAV from URL
```cpp
void wifi_status_callback(const char* status) {
    if (strcmp(status, "AP_STARTED") == 0) {
        // Play WAV when BLE is ready
        audio_play_wav_url("http://your-server.com/ble-ready.wav");
    } else if (strcmp(status, "WIFI_CONNECTED") == 0) {
        // Play WAV when WiFi connects
        audio_play_wav_url("http://your-server.com/wifi-connected.wav");
    }
}
```

### Integrate with AI Service
```cpp
// After WiFi is connected, request audio from AI service
void request_ai_audio(const char* text) {
    HTTPClient http;
    http.begin("https://your-backend.com/generate-speech");
    http.addHeader("Content-Type", "application/json");
    
    String payload = "{\"text\":\"" + String(text) + "\",\"format\":\"wav\"}";
    int httpCode = http.POST(payload);
    
    if (httpCode == HTTP_CODE_OK) {
        String audioUrl = http.getString();  // Backend returns URL to WAV file
        audio_play_wav_url(audioUrl.c_str());
    }
    
    http.end();
}
```

## WAV File Requirements

### Optimal Settings for ESP32-S3
- **Sample Rate**: 16000 Hz or 44100 Hz
- **Bit Depth**: 16-bit
- **Channels**: Mono (stereo will be converted to mono)
- **Format**: PCM (uncompressed)
- **File Size**: Keep under 1MB for smooth streaming

### Why These Settings?
- **16kHz**: Perfect for speech, smaller files
- **44.1kHz**: CD quality, better for music
- **16-bit**: Good quality, manageable size
- **Mono**: Saves bandwidth, sufficient for speech

## Next Steps

### Short Term (This Week)
1. ✅ Get WAV streaming working
2. Test with a simple WAV file
3. Test with ElevenLabs/OpenAI WAV output
4. Optimize buffer sizes for smooth playback

### Medium Term (Next Week)
1. Add MP3 decoder (try ESP8266Audio library)
2. Implement non-blocking audio playback
3. Add volume control
4. Add playback progress tracking

### Long Term (Future)
1. Implement audio caching in SPIFFS
2. Add playlist support
3. Implement audio mixing (play multiple sounds)
4. Add audio effects (fade in/out, etc.)

## Troubleshooting

### No Audio Output
- Check amplifier connections (GPIO 4, 5, 2)
- Verify amplifier has power (3.3V or 5V)
- Check speaker connections
- Verify WiFi is connected before streaming

### Choppy Audio
- Increase buffer size in audio.cpp
- Check WiFi signal strength
- Reduce sample rate to 16kHz
- Use shorter audio files

### HTTP Errors
- Verify URL is accessible from ESP32
- Check firewall settings
- Ensure server supports HTTP (not just HTTPS)
- For HTTPS, may need to add SSL certificate

## Resources

- [ElevenLabs API Docs](https://elevenlabs.io/docs/api-reference)
- [OpenAI TTS API Docs](https://platform.openai.com/docs/guides/text-to-speech)
- [ESP32 I2S Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/i2s.html)
- [WAV File Format](http://soundfile.sapp.org/doc/WaveFormat/)

