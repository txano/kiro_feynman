#include "audio.h"
#include <driver/i2s.h>
#include <esp_log.h>
#include <math.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <LittleFS.h>

static const char *TAG = "audio";
static bool littlefs_mounted = false;

// I2S configuration
#define I2S_NUM         I2S_NUM_0
#define I2S_BCK_IO      4
#define I2S_WS_IO       5
#define I2S_DO_IO       2
#define SAMPLE_RATE     44100
#define BITS_PER_SAMPLE 16
#define AMPLITUDE       10000  // Volume level (max 32767 for 16-bit)

// Background audio playback
static TaskHandle_t audio_task_handle = NULL;
static bool is_playing = false;
static char current_file[64] = {0};

// Audio task function
void audio_task(void* parameter) {
    while (true) {
        if (is_playing && strlen(current_file) > 0) {
            // Play the file (blocking within this task)
            File file = LittleFS.open(current_file, "r");
            if (file) {
                // Read RIFF header
                char riff[4];
                uint32_t file_size;
                char wave[4];
                
                file.read((uint8_t*)riff, 4);
                file.read((uint8_t*)&file_size, 4);
                file.read((uint8_t*)wave, 4);
                
                if (strncmp(riff, "RIFF", 4) == 0 && strncmp(wave, "WAVE", 4) == 0) {
                    // Find fmt chunk
                    uint16_t audio_format = 0;
                    uint16_t num_channels = 0;
                    uint32_t sample_rate = 0;
                    uint16_t bits_per_sample = 0;
                    
                    while (file.available()) {
                        char chunk_id[4];
                        uint32_t chunk_size;
                        
                        if (file.read((uint8_t*)chunk_id, 4) != 4) break;
                        if (file.read((uint8_t*)&chunk_size, 4) != 4) break;
                        
                        if (strncmp(chunk_id, "fmt ", 4) == 0) {
                            file.read((uint8_t*)&audio_format, 2);
                            file.read((uint8_t*)&num_channels, 2);
                            file.read((uint8_t*)&sample_rate, 4);
                            file.seek(file.position() + 6);
                            file.read((uint8_t*)&bits_per_sample, 2);
                            
                            if (chunk_size > 16) {
                                file.seek(file.position() + (chunk_size - 16));
                            }
                            break;
                        } else {
                            file.seek(file.position() + chunk_size);
                        }
                    }
                    
                    // Find data chunk
                    uint32_t data_size = 0;
                    while (file.available()) {
                        char chunk_id[4];
                        uint32_t chunk_size;
                        
                        if (file.read((uint8_t*)chunk_id, 4) != 4) break;
                        if (file.read((uint8_t*)&chunk_size, 4) != 4) break;
                        
                        if (strncmp(chunk_id, "data", 4) == 0) {
                            data_size = chunk_size;
                            break;
                        } else {
                            file.seek(file.position() + chunk_size);
                        }
                    }
                    
                    if (data_size > 0 && audio_format == 1) {
                        // Update sample rate
                        if (sample_rate != SAMPLE_RATE) {
                            i2s_set_sample_rates(I2S_NUM, sample_rate);
                        }
                        
                        // Play audio
                        const size_t buffer_size = 512;  // Smaller buffer for more responsive task
                        uint8_t* buffer = (uint8_t*)malloc(buffer_size);
                        if (buffer) {
                            size_t total_read = 0;
                            size_t bytes_written = 0;
                            
                            while (file.available() && total_read < data_size && is_playing) {
                                size_t to_read = min(buffer_size, data_size - total_read);
                                size_t bytes_read = file.read(buffer, to_read);
                                
                                if (bytes_read > 0) {
                                    if (num_channels == 2 && bits_per_sample == 16) {
                                        int16_t* samples = (int16_t*)buffer;
                                        size_t sample_count = bytes_read / 4;
                                        for (size_t i = 0; i < sample_count; i++) {
                                            samples[i] = (samples[i * 2] + samples[i * 2 + 1]) / 2;
                                        }
                                        i2s_write(I2S_NUM, buffer, sample_count * 2, &bytes_written, 100 / portTICK_PERIOD_MS);
                                    } else {
                                        i2s_write(I2S_NUM, buffer, bytes_read, &bytes_written, 100 / portTICK_PERIOD_MS);
                                    }
                                    total_read += bytes_read;
                                } else {
                                    break;
                                }
                                
                                // Yield to other tasks
                                vTaskDelay(1);
                            }
                            
                            free(buffer);
                        }
                        
                        // Restore sample rate
                        if (sample_rate != SAMPLE_RATE) {
                            i2s_set_sample_rates(I2S_NUM, SAMPLE_RATE);
                        }
                    }
                }
                
                file.close();
            }
            
            // Done playing
            is_playing = false;
            current_file[0] = '\0';
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// WAV file header structure
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t file_size;     // File size - 8
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmt_size;      // Format chunk size
    uint16_t audio_format;  // Audio format (1 = PCM)
    uint16_t num_channels;  // Number of channels
    uint32_t sample_rate;   // Sample rate
    uint32_t byte_rate;     // Byte rate
    uint16_t block_align;   // Block align
    uint16_t bits_per_sample; // Bits per sample
    char data[4];           // "data"
    uint32_t data_size;     // Data size
} wav_header_t;

void audio_init() {
    ESP_LOGI(TAG, "Initializing I2S audio on GPIO %d (DIN)", I2S_DO_IO);
    
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL));
    ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM, &pin_config));
    
    ESP_LOGI(TAG, "I2S initialized: %d Hz, GPIO %d", SAMPLE_RATE, I2S_DO_IO);
    
    // Initialize LittleFS
    ESP_LOGI(TAG, "Mounting LittleFS...");
    if (LittleFS.begin(true)) {
        littlefs_mounted = true;
        ESP_LOGI(TAG, "LittleFS mounted successfully");
    } else {
        ESP_LOGE(TAG, "LittleFS mount failed");
    }
    
    // Create audio playback task
    xTaskCreatePinnedToCore(
        audio_task,
        "audio_task",
        8192,  // Stack size
        NULL,
        1,     // Priority
        &audio_task_handle,
        0      // Core 0
    );
    ESP_LOGI(TAG, "Audio task created");
}

void audio_play_tone(uint16_t frequency_hz, uint32_t duration_ms) {
    ESP_LOGI(TAG, "Playing %d Hz tone for %lu ms", frequency_hz, duration_ms);
    
    uint32_t sample_count = (SAMPLE_RATE * duration_ms) / 1000;
    int16_t *samples = (int16_t *)malloc(sample_count * sizeof(int16_t));
    
    if (samples == NULL) {
        ESP_LOGE(TAG, "Failed to allocate sample buffer");
        return;
    }
    
    // Generate sine wave
    for (uint32_t i = 0; i < sample_count; i++) {
        float t = (float)i / SAMPLE_RATE;
        samples[i] = (int16_t)(AMPLITUDE * sin(2.0 * M_PI * frequency_hz * t));
    }
    
    size_t bytes_written = 0;
    i2s_write(I2S_NUM, samples, sample_count * sizeof(int16_t), &bytes_written, portMAX_DELAY);
    
    free(samples);
    ESP_LOGI(TAG, "Tone complete");
}

void audio_play_ble_ready_tone() {
    audio_play_tone(800, 200);  // 800 Hz for 200ms
}

void audio_play_wifi_connected_tone() {
    audio_play_tone(1200, 300);  // 1200 Hz for 300ms
}

bool audio_play_wav_url(const char* url) {
    ESP_LOGI(TAG, "Streaming WAV from: %s", url);
    
    HTTPClient http;
    http.begin(url);
    
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        ESP_LOGE(TAG, "HTTP GET failed: %d", httpCode);
        http.end();
        return false;
    }
    
    WiFiClient* stream = http.getStreamPtr();
    
    // Read WAV header
    wav_header_t header;
    if (stream->readBytes((char*)&header, sizeof(wav_header_t)) != sizeof(wav_header_t)) {
        ESP_LOGE(TAG, "Failed to read WAV header");
        http.end();
        return false;
    }
    
    // Validate WAV header
    if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0) {
        ESP_LOGE(TAG, "Invalid WAV file");
        http.end();
        return false;
    }
    
    ESP_LOGI(TAG, "WAV: %d Hz, %d channels, %d bits", 
             header.sample_rate, header.num_channels, header.bits_per_sample);
    
    // Update I2S sample rate if needed
    if (header.sample_rate != SAMPLE_RATE) {
        i2s_set_sample_rates(I2S_NUM, header.sample_rate);
    }
    
    // Stream audio data
    const size_t buffer_size = 1024;
    uint8_t* buffer = (uint8_t*)malloc(buffer_size);
    if (!buffer) {
        ESP_LOGE(TAG, "Failed to allocate buffer");
        http.end();
        return false;
    }
    
    size_t total_read = 0;
    size_t bytes_written = 0;
    
    while (stream->available() && total_read < header.data_size) {
        size_t to_read = min(buffer_size, header.data_size - total_read);
        size_t bytes_read = stream->readBytes(buffer, to_read);
        
        if (bytes_read > 0) {
            // Convert stereo to mono if needed
            if (header.num_channels == 2 && header.bits_per_sample == 16) {
                int16_t* samples = (int16_t*)buffer;
                size_t sample_count = bytes_read / 4;  // 4 bytes per stereo sample
                for (size_t i = 0; i < sample_count; i++) {
                    samples[i] = (samples[i * 2] + samples[i * 2 + 1]) / 2;  // Average L+R
                }
                i2s_write(I2S_NUM, buffer, sample_count * 2, &bytes_written, portMAX_DELAY);
            } else {
                i2s_write(I2S_NUM, buffer, bytes_read, &bytes_written, portMAX_DELAY);
            }
            total_read += bytes_read;
        } else {
            break;
        }
    }
    
    free(buffer);
    http.end();
    
    // Restore original sample rate
    if (header.sample_rate != SAMPLE_RATE) {
        i2s_set_sample_rates(I2S_NUM, SAMPLE_RATE);
    }
    
    ESP_LOGI(TAG, "WAV playback complete: %d bytes", total_read);
    return true;
}

bool audio_play_wav_file(const char* filename) {
    if (!littlefs_mounted) {
        ESP_LOGE(TAG, "LittleFS not mounted");
        return false;
    }
    
    if (is_playing) {
        ESP_LOGW(TAG, "Already playing audio");
        return false;
    }
    
    ESP_LOGI(TAG, "Queueing WAV file: %s", filename);
    
    // Set the file to play
    strncpy(current_file, filename, sizeof(current_file) - 1);
    current_file[sizeof(current_file) - 1] = '\0';
    is_playing = true;
    
    return true;
}

void audio_play_mp3(const char* filename) {
    ESP_LOGW(TAG, "MP3 playback not yet implemented - playing tone instead");
    audio_play_tone(1000, 500);
}

void audio_loop() {
    // No-op for tone-based audio
}

bool audio_is_playing() {
    return is_playing;
}
