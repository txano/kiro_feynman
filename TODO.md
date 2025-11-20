# TODO List

## 🔥 Immediate (Phase 1 Testing)

- [x] Test on physical ESP32-S3-Matrix hardware
- [x] Verify LED matrix displays correctly
- [x] Test BLE provisioning with phone app
- [x] Verify WiFi connection and auto-reconnect
- [x] Document any issues or bugs found
- [ ] Test power consumption and heat generation

## 📋 Phase 2: Audio Streaming (In Progress) 🚧

### Research & Planning
- [x] Plan I2S pin configuration (GPIO 4, 5, 2)
- [x] Test I2S driver with tone generation
- [ ] Choose audio codec (MP3 vs AAC vs WAV)
- [ ] Select audio streaming library (ESP32-audioI2S recommended)
- [ ] Determine buffer sizes and sample rates
- [ ] Find sample audio URLs for testing

### Implementation
- [x] Add I2S driver initialization
- [x] Implement basic tone generation
- [x] Add audio feedback for BLE ready (800 Hz tone)
- [x] Add audio feedback for WiFi connected (1200 Hz tone)
- [ ] Implement HTTP audio streaming client
- [ ] Add audio decoder (MP3/AAC)
- [ ] Implement buffer management
- [ ] Add basic playback controls (play/pause)
- [ ] Integrate with LED matrix (show playing status)
- [ ] Test with sample audio files

### Testing
- [ ] Test audio quality at different bitrates
- [ ] Verify no buffer underruns
- [ ] Test streaming stability over WiFi
- [ ] Measure latency
- [ ] Test with different audio formats

## 📋 Phase 3: User Interface

### Hardware Integration
- [ ] Add button GPIO configuration (GPIO 1, 2, 3)
- [ ] Implement button debouncing
- [ ] Add potentiometer ADC reading (GPIO 4)
- [ ] Test button responsiveness
- [ ] Calibrate volume control

### Software Implementation
- [ ] Create button event handler
- [ ] Implement story selection UI
- [ ] Add volume control logic
- [ ] Create LED matrix UI elements
- [ ] Add visual feedback for all interactions

### Features
- [ ] Story selection menu
- [ ] Play/Pause button
- [ ] Next/Previous story buttons
- [ ] Volume up/down via potentiometer
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
- [ ] No factory reset mechanism
- [ ] LED matrix brightness could damage board if set too high
- [ ] No error recovery for failed WiFi connection

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
