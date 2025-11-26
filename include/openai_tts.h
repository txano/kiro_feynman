#ifndef OPENAI_TTS_H
#define OPENAI_TTS_H

#include <stdint.h>

// Initialize OpenAI TTS client
void openai_tts_init(const char* api_key);

// Request TTS from OpenAI and play the audio
// text: The text to convert to speech
// Returns true if successful, false otherwise
bool openai_tts_speak(const char* text);

// Test DNS resolution with well-known domains
void openai_tts_test_dns();

#endif // OPENAI_TTS_H
