# TODO List

## 🔥 Immediate (Phase 1 Testing)

- [x] Test on physical ESP32-S3-Matrix hardware
- [x] Verify LED matrix displays correctly
- [x] Test BLE provisioning with phone app
- [x] Verify WiFi connection and auto-reconnect
- [x] Document any issues or bugs found
- [ ] Test power consumption and heat generation

## 📋 Phase 2: Audio Streaming ✅ COMPLETE

### Research & Planning
- [x] Plan I2S pin configuration (GPIO 4, 5, 2)
- [x] Test I2S driver with tone generation
- [x] Choose audio codec (MP3 selected)
- [x] Select audio streaming library (ESP8266Audio)
- [x] Determine buffer sizes and sample rates
- [x] Find sample audio URLs for testing

### Implementation
- [x] Add I2S driver initialization
- [x] Implement basic tone generation
- [x] Add audio feedback for BLE ready (ready.mp3)
- [x] Add audio feedback for WiFi connected (connected.mp3)
- [x] Implement HTTPS audio streaming client
- [x] Add MP3 decoder (Helix)
- [x] Implement buffer management
- [x] Add basic playback controls (play button)
- [x] Add volume control (potentiometer)
- [x] Add reset button for WiFi credentials
- [x] Integrate with LED matrix (show playing status)
- [x] Test with sample audio files

### Testing
- [x] Test audio quality at different bitrates
- [x] Verify no buffer underruns
- [x] Test streaming stability over WiFi
- [x] Test with large files (2MB+)
- [x] Test HTTPS downloads from Supabase

## 📋 Phase 3: User Interface (In Progress) 🚧

### Hardware Integration
- [x] Add button GPIO configuration (GPIO 33, 34)
- [x] Implement button debouncing
- [x] Add potentiometer ADC reading (GPIO 6)
- [x] Test button responsiveness
- [x] Calibrate volume control
- [ ] Add additional buttons (GPIO 1, 3)

### Software Implementation
- [x] Create button event handler
- [x] Add volume control logic
- [x] Add reset button functionality
- [ ] Implement story selection UI
- [ ] Create LED matrix UI elements
- [ ] Add visual feedback for all interactions

### Features
- [x] Play button (GPIO 33)
- [x] Reset button (GPIO 34 - hold 5s)
- [x] Volume control via potentiometer (GPIO 6)
- [ ] Story selection menu
- [ ] Next/Previous story buttons
- [ ] Visual volume indicator on LED matrix

## 📋 Phase 4: Cloud Integration

### Backend Service
- [ ] Design API endpoints
- [ ] Implement user authentication
- [ ] Create story catalog API
- [ ] Set up audio streaming service
- [ ] Implement device registration

### Device Integration
- [ ] Add HTTPS client
- [ ] Implement device authentication
- [ ] Add story catalog fetching
- [ ] Implement story metadata parsing
- [ ] Add local caching (SPIFFS)

### Features
- [ ] User account linking
- [ ] Story personalization
- [ ] Parental controls
- [ ] Usage analytics
- [ ] Content recommendations

## 🔧 Improvements & Optimizations

### Code Quality
- [ ] Add error recovery mechanisms
- [ ] Implement watchdog timer
- [ ] Add comprehensive logging
- [ ] Create unit tests (if applicable)
- [ ] Add code documentation
- [ ] Refactor for better modularity

### User Experience
- [ ] Add timeout for provisioning
- [ ] Implement factory reset button combo
- [ ] Add low battery indicator
- [ ] Create startup animation
- [ ] Add sleep mode for power saving
- [ ] Implement graceful error messages

### Security
- [ ] Generate unique PoP per device
- [ ] Implement secure storage for credentials
- [ ] Add HTTPS certificate validation
- [ ] Implement device authentication tokens
- [ ] Add encrypted communication with cloud

### Performance
- [ ] Optimize memory usage
- [ ] Reduce boot time
- [ ] Improve WiFi reconnection speed
- [ ] Optimize LED matrix refresh rate
- [ ] Reduce audio latency

## 🐛 Known Issues

- [ ] Hardcoded PoP (should be unique per device)
- [ ] No provisioning timeout
- [x] No factory reset mechanism - FIXED (GPIO 34 reset button)
- [ ] LED matrix brightness could damage board if set too high
- [ ] No error recovery for failed WiFi connection
- [ ] HTTPS certificate validation disabled (insecure mode for development)
- [ ] OpenAI TTS module not integrated (placeholder only)

## 📚 Documentation

- [ ] Add API documentation
- [ ] Create hardware assembly guide
- [ ] Write user manual
- [ ] Add troubleshooting guide
- [ ] Create video tutorials
- [ ] Document cloud service API

## 🚀 Future Features (Nice to Have)

- [ ] Multi-language support
- [ ] Bluetooth audio output option
- [ ] SD card support for offline stories
- [ ] Night light mode
- [ ] Timer/alarm functionality
- [ ] Interactive stories with button choices
- [ ] Story progress tracking
- [ ] Favorites list
- [ ] Parental app for remote control
- [ ] Voice commands (if microphone added)

## 🔬 Research Topics

- [ ] Battery power option and charging circuit
- [ ] Case design and 3D printing
- [ ] Child-safe materials and certifications
- [ ] Audio quality optimization for speech
- [ ] Power consumption optimization
- [ ] Manufacturing and scaling considerations
- [ ] Consider BLE provisioning as alternative to SoftAP

---

## Priority Legend

- 🔥 Critical / Immediate
- 📋 Planned / Next Phase
- 🔧 Improvement / Enhancement
- 🐛 Bug / Issue
- 📚 Documentation
- 🚀 Future / Nice to Have
- 🔬 Research / Investigation
