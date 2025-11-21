#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

// Initialize I2S audio and MP3 player
void audio_init();

// Play a tone at specified frequency for duration
void audio_play_tone(uint16_t frequency_hz, uint32_t duration_ms);

// Predefined tones for different events
void audio_play_ble_ready_tone();
void audio_play_wifi_connected_tone();

// MP3 playback functions
void audio_play_mp3(const char* filename);
void audio_loop();  // Call in main loop to handle audio playback
bool audio_is_playing();

#endif // AUDIO_H
