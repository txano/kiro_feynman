#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

// Initialize I2S audio
void audio_init();

// Play a tone at specified frequency for duration
void audio_play_tone(uint16_t frequency_hz, uint32_t duration_ms);

// Predefined tones for different events
void audio_play_ble_ready_tone();
void audio_play_wifi_connected_tone();

#endif // AUDIO_H
