#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

// Initialize I2S audio and MP3 player
void audio_init();

// Audio file playback functions
bool audio_play_mp3_file(const char* filename);  // Play MP3 from LittleFS
void audio_loop();  // Call in main loop to handle audio playback
bool audio_is_playing();

// Playback control
void audio_pause();
void audio_resume();
void audio_toggle_pause();
void audio_stop();
bool audio_is_paused();

// Volume control (0.0 to 1.0)
void audio_set_volume(float volume);

// Non-blocking download functions
bool audio_start_download(const char* url);  // Start async download
bool audio_is_downloading();                  // Check if download in progress
bool audio_download_complete();               // Check if download finished successfully
bool audio_download_failed();                 // Check if download failed
void audio_reset_download_state();            // Reset download state machine

// Legacy blocking download (kept for compatibility)
bool audio_download_mp3_url(const char* url);

#endif // AUDIO_H
