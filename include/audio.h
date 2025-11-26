#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

// Initialize I2S audio and MP3 player
void audio_init();

// Audio file playback functions
bool audio_play_mp3_file(const char* filename);  // Play MP3 from LittleFS
bool audio_download_mp3_url(const char* url);  // Download MP3 from URL (doesn't play)
void audio_loop();  // Call in main loop to handle audio playback
bool audio_is_playing();

// Volume control (0.0 to 1.0)
void audio_set_volume(float volume);
void audio_enable_fade(float start_vol, float end_vol, unsigned long duration_ms);

#endif // AUDIO_H
