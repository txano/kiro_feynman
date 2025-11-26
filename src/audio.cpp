#include "audio.h"
#include <LittleFS.h>
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include <driver/i2s.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <lwip/ip4_addr.h>
#include <lwip/dns.h>
#include <lwip/netdb.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <math.h>

static const char *TAG = "audio";
static bool littlefs_mounted = false;

// ESP8266Audio objects for MP3 playback
static AudioGeneratorMP3 *mp3 = NULL;
static AudioFileSourceLittleFS *file_source = NULL;
static AudioOutputI2S *out = NULL;

// I2S configuration
#define I2S_NUM         I2S_NUM_0
#define I2S_BCK_IO      4
#define I2S_WS_IO       5
#define I2S_DO_IO       2
#define SAMPLE_RATE     44100
#define BITS_PER_SAMPLE 16
#define AMPLITUDE       10000  // Volume level (max 32767 for 16-bit)

// Audio playback state
static bool is_playing = false;
static bool is_paused = false;
static float current_volume = 1.0;  // 100% by default

// Download state machine for non-blocking downloads
typedef enum {
    DOWNLOAD_IDLE,
    DOWNLOAD_CONNECTING,
    DOWNLOAD_IN_PROGRESS,
    DOWNLOAD_COMPLETE,
    DOWNLOAD_FAILED
} download_state_t;

static download_state_t download_state = DOWNLOAD_IDLE;
static WiFiClientSecure *download_client = NULL;
static HTTPClient *download_http = NULL;
static File download_file;
static int download_total_bytes = 0;
static int download_received_bytes = 0;
static String download_url = "";



void audio_init() {
    ESP_LOGI(TAG, "Initializing I2S audio on GPIO %d (DIN)", I2S_DO_IO);
    
    // Initialize LittleFS
    ESP_LOGI(TAG, "Mounting LittleFS...");
    if (LittleFS.begin(true)) {
        littlefs_mounted = true;
        ESP_LOGI(TAG, "LittleFS mounted successfully");
    } else {
        ESP_LOGE(TAG, "LittleFS mount failed");
    }
    
    // Initialize ESP8266Audio output (it will handle I2S driver)
    out = new AudioOutputI2S(I2S_NUM, 0);  // Use I2S_NUM_0, internal DAC off
    out->SetPinout(I2S_BCK_IO, I2S_WS_IO, I2S_DO_IO);
    out->SetGain(1.0);  // 100% volume
    
    ESP_LOGI(TAG, "Audio initialized with ESP8266Audio library");
}

// WAV playback removed - using MP3 only

bool audio_play_mp3_file(const char* filename) {
    if (!littlefs_mounted) {
        ESP_LOGE(TAG, "LittleFS not mounted");
        return false;
    }
    
    if (is_playing) {
        ESP_LOGW(TAG, "Already playing audio");
        return false;
    }
    
    ESP_LOGI(TAG, "Playing MP3 file: %s", filename);
    
    // Clean up previous file source if exists
    if (file_source != NULL) {
        delete file_source;
        file_source = NULL;
    }
    
    // Clean up previous MP3 decoder if exists
    if (mp3 != NULL) {
        delete mp3;
        mp3 = NULL;
    }
    
    // Create new file source and MP3 decoder
    file_source = new AudioFileSourceLittleFS(filename);
    mp3 = new AudioGeneratorMP3();
    
    if (mp3->begin(file_source, out)) {
        is_playing = true;
        is_paused = false;
        ESP_LOGI(TAG, "MP3 playback started");
        return true;
    } else {
        ESP_LOGE(TAG, "Failed to start MP3 playback");
        delete mp3;
        delete file_source;
        mp3 = NULL;
        file_source = NULL;
        return false;
    }
}

// Helper to extract hostname from URL
static bool extract_hostname(const char* url, char* hostname, size_t hostname_size) {
    // Skip protocol (https:// or http://)
    const char* start = strstr(url, "://");
    if (start == NULL) {
        return false;
    }
    start += 3;  // Skip "://"
    
    // Find end of hostname (either / or end of string)
    const char* end = strchr(start, '/');
    size_t len = end ? (size_t)(end - start) : strlen(start);
    
    if (len >= hostname_size) {
        return false;
    }
    
    strncpy(hostname, start, len);
    hostname[len] = '\0';
    return true;
}

// Start a non-blocking download
bool audio_start_download(const char* url) {
    if (is_playing) {
        ESP_LOGW(TAG, "Already playing audio");
        return false;
    }
    
    if (download_state != DOWNLOAD_IDLE) {
        ESP_LOGW(TAG, "Download already in progress");
        return false;
    }
    
    ESP_LOGI(TAG, "Starting download from URL: %s", url);
    ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
    
    // Extract hostname for DNS resolution
    char hostname[128];
    if (!extract_hostname(url, hostname, sizeof(hostname))) {
        ESP_LOGE(TAG, "Failed to extract hostname from URL");
        return false;
    }
    
    // Resolve DNS using lwIP (works with ESP-IDF WiFi)
    ESP_LOGI(TAG, "Resolving DNS for: %s", hostname);
    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    struct addrinfo *result = NULL;
    int err = getaddrinfo(hostname, "443", &hints, &result);
    
    if (err != 0 || result == NULL) {
        ESP_LOGE(TAG, "DNS resolution failed: %d", err);
        if (result) freeaddrinfo(result);
        return false;
    }
    
    struct sockaddr_in *addr = (struct sockaddr_in *)result->ai_addr;
    char ip_str[16];
    inet_ntoa_r(addr->sin_addr, ip_str, sizeof(ip_str));
    ESP_LOGI(TAG, "DNS resolved to: %s", ip_str);
    freeaddrinfo(result);
    
    // Store URL for later use
    download_url = String(url);
    
    // Delete old temp file if it exists
    if (LittleFS.exists("/temp_stream.mp3")) {
        LittleFS.remove("/temp_stream.mp3");
    }
    
    // Open file for writing
    download_file = LittleFS.open("/temp_stream.mp3", "w");
    if (!download_file) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return false;
    }
    
    // Create client and HTTP objects
    download_client = new WiFiClientSecure();
    download_client->setInsecure();
    
    download_http = new HTTPClient();
    download_http->begin(*download_client, url);
    
    // Start the request
    ESP_LOGI(TAG, "Starting HTTP GET request...");
    int httpCode = download_http->GET();
    
    ESP_LOGI(TAG, "HTTP response code: %d", httpCode);
    
    if (httpCode == HTTP_CODE_OK) {
        download_total_bytes = download_http->getSize();
        download_received_bytes = 0;
        download_state = DOWNLOAD_IN_PROGRESS;
        ESP_LOGI(TAG, "Download started, total size: %d bytes", download_total_bytes);
        return true;
    } else {
        ESP_LOGE(TAG, "HTTP error: %d - %s", httpCode, download_http->errorToString(httpCode).c_str());
        download_file.close();
        download_http->end();
        delete download_http;
        delete download_client;
        download_http = NULL;
        download_client = NULL;
        download_state = DOWNLOAD_FAILED;
        return false;
    }
}

// Process download in chunks (call from loop)
void audio_download_loop() {
    if (download_state != DOWNLOAD_IN_PROGRESS) {
        return;
    }
    
    WiFiClient *stream = download_http->getStreamPtr();
    if (stream == NULL) {
        ESP_LOGE(TAG, "Stream is null");
        download_state = DOWNLOAD_FAILED;
        return;
    }
    
    // Read available data in small chunks to keep LED responsive
    int available = stream->available();
    if (available > 0) {
        // Read up to 1KB at a time to keep loop responsive
        uint8_t buffer[1024];
        int toRead = min(available, (int)sizeof(buffer));
        int bytesRead = stream->readBytes(buffer, toRead);
        
        if (bytesRead > 0) {
            download_file.write(buffer, bytesRead);
            download_received_bytes += bytesRead;
            
            // Log progress every 100KB
            static int last_logged = 0;
            if (download_received_bytes - last_logged >= 102400) {
                ESP_LOGI(TAG, "Download progress: %d / %d bytes (%.1f%%)", 
                         download_received_bytes, download_total_bytes,
                         download_total_bytes > 0 ? (download_received_bytes * 100.0 / download_total_bytes) : 0);
                last_logged = download_received_bytes;
            }
        }
    }
    
    // Check if download is complete
    if (!stream->connected() && stream->available() == 0) {
        download_file.close();
        download_http->end();
        delete download_http;
        delete download_client;
        download_http = NULL;
        download_client = NULL;
        
        if (download_received_bytes > 0) {
            download_state = DOWNLOAD_COMPLETE;
            ESP_LOGI(TAG, "Download complete: %d bytes", download_received_bytes);
        } else {
            download_state = DOWNLOAD_FAILED;
            ESP_LOGE(TAG, "Download failed: no data received");
        }
    }
}

// Check download status
bool audio_is_downloading() {
    return download_state == DOWNLOAD_IN_PROGRESS;
}

bool audio_download_complete() {
    return download_state == DOWNLOAD_COMPLETE;
}

bool audio_download_failed() {
    return download_state == DOWNLOAD_FAILED;
}

void audio_reset_download_state() {
    download_state = DOWNLOAD_IDLE;
    download_received_bytes = 0;
    download_total_bytes = 0;
}

// Legacy blocking download (kept for compatibility)
bool audio_download_mp3_url(const char* url) {
    if (!audio_start_download(url)) {
        return false;
    }
    
    // Block until complete (legacy behavior)
    while (download_state == DOWNLOAD_IN_PROGRESS) {
        audio_download_loop();
        delay(1);
    }
    
    bool success = (download_state == DOWNLOAD_COMPLETE);
    audio_reset_download_state();
    return success;
}

void audio_set_volume(float volume) {
    // Clamp volume between 0.0 and 1.0
    if (volume < 0.0) volume = 0.0;
    if (volume > 1.0) volume = 1.0;
    
    current_volume = volume;
    
    if (out != NULL) {
        out->SetGain(volume);
        ESP_LOGI(TAG, "Volume set to %.0f%%", volume * 100);
    }
}

void audio_pause() {
    if (mp3 != NULL && mp3->isRunning() && !is_paused) {
        is_paused = true;
        ESP_LOGI(TAG, "Audio paused");
    }
}

void audio_resume() {
    if (mp3 != NULL && is_paused) {
        is_paused = false;
        ESP_LOGI(TAG, "Audio resumed");
    }
}

void audio_toggle_pause() {
    if (is_paused) {
        audio_resume();
    } else {
        audio_pause();
    }
}

void audio_stop() {
    if (mp3 != NULL) {
        mp3->stop();
        ESP_LOGI(TAG, "Audio stopped");
    }
    is_playing = false;
    is_paused = false;
}

bool audio_is_paused() {
    return is_paused;
}

void audio_loop() {
    // Handle non-blocking download
    audio_download_loop();
    
    // Handle MP3 playback
    if (mp3 != NULL && mp3->isRunning()) {
        // Skip processing if paused
        if (is_paused) {
            return;
        }
        
        if (!mp3->loop()) {
            // MP3 finished
            mp3->stop();
            ESP_LOGI(TAG, "MP3 playback complete");
            is_playing = false;
            is_paused = false;
        }
    }
}

bool audio_is_playing() {
    if (mp3 != NULL && mp3->isRunning()) {
        return true;
    }
    return is_playing;
}
