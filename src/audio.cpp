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
static unsigned long playback_start_time = 0;
static float current_volume = 1.0;  // 100% by default
static bool volume_fade_enabled = false;
static float fade_start_volume = 0.2;  // 20%
static float fade_end_volume = 0.5;    // 50%
static unsigned long fade_duration = 10000;  // 10 seconds



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
        
        // Start fade timer if fade is enabled
        if (volume_fade_enabled) {
            playback_start_time = millis();
            ESP_LOGI(TAG, "MP3 playback started with volume fade");
        } else {
            playback_start_time = 0;
            ESP_LOGI(TAG, "MP3 playback started");
        }
        
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

bool audio_download_mp3_url(const char* url) {
    if (is_playing) {
        ESP_LOGW(TAG, "Already playing audio");
        return false;
    }
    
    ESP_LOGI(TAG, "Downloading MP3 from URL: %s", url);
    ESP_LOGI(TAG, "Free heap before download: %d bytes", esp_get_free_heap_size());
    
    // Check network connectivity first
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif == NULL) {
        ESP_LOGE(TAG, "Network interface not found!");
        return false;
    }
    
    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
        ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip_info.ip));
        ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&ip_info.gw));
        ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&ip_info.netmask));
    }
    
    // Check DNS servers
    esp_netif_dns_info_t dns_info;
    if (esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info) == ESP_OK) {
        ESP_LOGI(TAG, "DNS Main: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
    }
    if (esp_netif_get_dns_info(netif, ESP_NETIF_DNS_BACKUP, &dns_info) == ESP_OK) {
        ESP_LOGI(TAG, "DNS Backup: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
    }
    
    // Try to resolve DNS using lwIP directly (works with ESP-IDF WiFi)
    ESP_LOGI(TAG, "Attempting DNS resolution using lwIP...");
    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    struct addrinfo *result = NULL;
    int err = getaddrinfo("mdnwjeafufvrrxphotzv.supabase.co", "443", &hints, &result);
    
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
    
    // Now try HTTP request
    ESP_LOGI(TAG, "Creating WiFiClientSecure...");
    WiFiClientSecure client;
    client.setInsecure();
    ESP_LOGI(TAG, "WiFiClientSecure created with insecure mode");
    
    HTTPClient http;
    ESP_LOGI(TAG, "Calling http.begin()...");
    http.begin(client, url);
    
    ESP_LOGI(TAG, "Starting HTTP GET request...");
    int httpCode = http.GET();
    
    ESP_LOGI(TAG, "HTTP response code: %d", httpCode);
    
    if (httpCode == HTTP_CODE_OK) {
        // Open file for writing
        File file = LittleFS.open("/temp_stream.mp3", "w");
        if (!file) {
            ESP_LOGE(TAG, "Failed to open file for writing");
            http.end();
            return false;
        }
        
        // Use writeToStream like the working example
        ESP_LOGI(TAG, "Writing stream to file...");
        int written = http.writeToStream(&file);
        file.close();
        
        ESP_LOGI(TAG, "File downloaded and saved: %d bytes", written);
        ESP_LOGI(TAG, "Free heap after download: %d bytes", esp_get_free_heap_size());
        
        http.end();
        
        if (written > 0) {
            ESP_LOGI(TAG, "File downloaded successfully: %d bytes", written);
            return true;
        } else {
            ESP_LOGE(TAG, "No data written to file");
            return false;
        }
    } else {
        ESP_LOGE(TAG, "Failed to download file, HTTP error code: %d", httpCode);
        ESP_LOGE(TAG, "Error message: %s", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }
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

void audio_enable_fade(float start_vol, float end_vol, unsigned long duration_ms) {
    fade_start_volume = start_vol;
    fade_end_volume = end_vol;
    fade_duration = duration_ms;
    volume_fade_enabled = true;
    
    // Set initial volume
    audio_set_volume(start_vol);
    
    ESP_LOGI(TAG, "Fade enabled: %.0f%% -> %.0f%% over %lu ms", 
             start_vol * 100, end_vol * 100, duration_ms);
}

void audio_loop() {
    // Handle MP3 playback
    if (mp3 != NULL && mp3->isRunning()) {
        // Handle volume fade if enabled
        if (volume_fade_enabled && playback_start_time > 0) {
            unsigned long elapsed = millis() - playback_start_time;
            
            if (elapsed < fade_duration) {
                // Calculate fade progress (0.0 to 1.0)
                float progress = (float)elapsed / (float)fade_duration;
                
                // Linear interpolation between start and end volume
                float target_volume = fade_start_volume + (fade_end_volume - fade_start_volume) * progress;
                
                // Only update if volume changed significantly (avoid too many updates)
                if (abs(target_volume - current_volume) > 0.01) {
                    audio_set_volume(target_volume);
                }
            } else if (elapsed >= fade_duration && current_volume != fade_end_volume) {
                // Fade complete, set final volume
                audio_set_volume(fade_end_volume);
                volume_fade_enabled = false;  // Disable fade
            }
        }
        
        if (!mp3->loop()) {
            // MP3 finished
            mp3->stop();
            ESP_LOGI(TAG, "MP3 playback complete");
            is_playing = false;
            volume_fade_enabled = false;
            playback_start_time = 0;
        }
    }
}

bool audio_is_playing() {
    if (mp3 != NULL && mp3->isRunning()) {
        return true;
    }
    return is_playing;
}
