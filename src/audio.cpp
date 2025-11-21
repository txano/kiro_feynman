#include "audio.h"
#include <driver/i2s.h>
#include <esp_log.h>
#include <math.h>

static const char *TAG = "audio";

// I2S configuration
#define I2S_NUM         I2S_NUM_0
#define I2S_BCK_IO      4
#define I2S_WS_IO       5
#define I2S_DO_IO       2
#define SAMPLE_RATE     44100
#define BITS_PER_SAMPLE 16
#define AMPLITUDE       10000  // Volume level (max 32767 for 16-bit)

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

void audio_play_mp3(const char* filename) {
    ESP_LOGW(TAG, "MP3 playback not yet implemented - playing tone instead");
    audio_play_tone(1000, 500);
}

void audio_loop() {
    // No-op for tone-based audio
}

bool audio_is_playing() {
    return false;  // Tones are blocking, so always return false
}
