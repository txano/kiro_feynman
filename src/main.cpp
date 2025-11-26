#include <Arduino.h>
#include <string.h>
#include "led_matrix.h"
#include "wifi_provisioning.h"
#include "audio.h"
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <lwip/ip4_addr.h>
#include <LittleFS.h>
#include <WiFi.h>

static const char *TAG = "main";
static bool wifi_connected = false;
static bool wifi_connecting = false;
static bool ble_ready = false;
static char ip_display_text[50] = "No WiFi";
static unsigned long last_status_check = 0;
static unsigned long last_ready_sound = 0;
static bool ble_ready_tone_played = false;
static bool wifi_connected_tone_played = false;
static bool play_connected_sound = false;  // Flag to play connected sound in main loop
static bool download_test_file = false;  // Flag to download test MP3 after WiFi connects
static unsigned long wifi_connected_time = 0;  // Time when WiFi connected

#define READY_SOUND_INTERVAL 10000  // Play ready.mp3 every 10 seconds

// Reset button configuration (External button on GPIO 34)
#define RESET_BUTTON_PIN 34
#define RESET_HOLD_TIME 5000  // Hold for 5 seconds to reset WiFi credentials
#define BUTTON_DEBOUNCE_TIME 50  // 50ms debounce
static unsigned long button_press_start = 0;
static bool button_was_pressed = false;
static unsigned long last_button_check = 0;

// Potentiometer configuration (GPIO 6 - ADC1_CH5)
#define VOLUME_POT_PIN 6
#define VOLUME_READ_INTERVAL 200  // Read every 200ms
static unsigned long last_volume_read = 0;
static int last_volume_value = -1;

// Play button configuration (GPIO 33)
#define PLAY_BUTTON_PIN 33
#define PLAY_BUTTON_STOP_HOLD_TIME 4000  // Hold 4 seconds to stop
static bool play_button_last_state = HIGH;
static unsigned long play_button_press_start = 0;
static bool play_button_was_pressed = false;
static bool file_downloaded = false;
static bool file_ready_to_play = false;
static bool download_started = false;

// Callback for WiFi provisioning status updates
void wifi_status_callback(const char* status) {
    ESP_LOGI(TAG, "WiFi Status: %s", status);
    
    if (strcmp(status, "AP_STARTED") == 0) {
        wifi_connected = false;
        wifi_connecting = false;
        ble_ready = true;
        strcpy(ip_display_text, "BLE Ready");
        if (!ble_ready_tone_played) {
            ble_ready_tone_played = true;
            last_ready_sound = millis();
        }
    } else if (strcmp(status, "AP_CONNECTED") == 0) {
        // Just log, don't block
    } else if (strcmp(status, "WIFI_CONNECTING") == 0) {
        wifi_connecting = true;
        wifi_connected = false;
        strcpy(ip_display_text, "Connecting...");
    } else if (strcmp(status, "WIFI_CONNECTED") == 0) {
        wifi_connected = true;
        wifi_connecting = false;
        ble_ready = false;
        strcpy(ip_display_text, "Getting IP...");
        
        // Configure Google DNS servers explicitly
        ESP_LOGI(TAG, "Configuring DNS servers: 8.8.8.8, 8.8.4.4");
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif != NULL) {
            esp_netif_dns_info_t dns_info;
            dns_info.ip.u_addr.ip4.addr = ipaddr_addr("8.8.8.8");
            dns_info.ip.type = IPADDR_TYPE_V4;
            esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
            
            dns_info.ip.u_addr.ip4.addr = ipaddr_addr("8.8.4.4");
            esp_netif_set_dns_info(netif, ESP_NETIF_DNS_BACKUP, &dns_info);
            ESP_LOGI(TAG, "DNS servers configured");
        }
        
        if (!wifi_connected_tone_played) {
            // Set flag to play connected sound first
            play_connected_sound = true;
            wifi_connected_time = millis();
            wifi_connected_tone_played = true;
        }
        
        // Synchronize time via NTP for HTTPS certificate validation
        ESP_LOGI(TAG, "Synchronizing time via NTP...");
        if (wifi_sync_time()) {
            ESP_LOGI(TAG, "Time synchronized successfully");
        } else {
            ESP_LOGW(TAG, "Failed to sync time, HTTPS may not work");
        }
        
        // IP will be updated in loop
    } else if (strcmp(status, "ERROR") == 0) {
        // Only show error if we're not already connected
        if (!wifi_connected) {
            wifi_connecting = false;
            strcpy(ip_display_text, "Retrying...");
        }
    }
}

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(1000);
    
    ESP_LOGI(TAG, "=================================");
    ESP_LOGI(TAG, "Story Device - Starting...");
    ESP_LOGI(TAG, "=================================");
    
    // Initialize LED matrix
    ESP_LOGI(TAG, "Initializing LED matrix...");
    led_matrix_init();
    led_matrix_show_text("INIT");
    delay(1000);
    
    // Initialize audio
    ESP_LOGI(TAG, "Initializing audio...");
    audio_init();
    
    // Initialize reset button (BOOT button)
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    ESP_LOGI(TAG, "Reset button initialized on GPIO %d", RESET_BUTTON_PIN);
    ESP_LOGI(TAG, "Hold BOOT button for %d seconds to reset WiFi credentials", RESET_HOLD_TIME / 1000);
    
    // Initialize potentiometer for volume control
    pinMode(VOLUME_POT_PIN, INPUT);
    analogSetAttenuation(ADC_11db);  // 0-3.3V range
    ESP_LOGI(TAG, "Volume potentiometer initialized on GPIO %d (ADC)", VOLUME_POT_PIN);
    
    // Initialize play button
    pinMode(PLAY_BUTTON_PIN, INPUT_PULLUP);
    ESP_LOGI(TAG, "Play button initialized on GPIO %d", PLAY_BUTTON_PIN);
    
    // Initialize WiFi provisioning
    ESP_LOGI(TAG, "Initializing WiFi provisioning...");
    wifi_prov_init(wifi_status_callback);
    
    // Start provisioning (will auto-connect if already provisioned)
    ESP_LOGI(TAG, "Starting WiFi provisioning...");
    wifi_prov_start();
    
    ESP_LOGI(TAG, "Setup complete!");
}

void loop() {
    unsigned long now = millis();
    
    // Check reset button (BOOT button) - active LOW with debouncing
    bool button_pressed = false;
    if (now - last_button_check >= BUTTON_DEBOUNCE_TIME) {
        button_pressed = (digitalRead(RESET_BUTTON_PIN) == LOW);
        last_button_check = now;
    } else {
        // Use previous state during debounce period
        button_pressed = button_was_pressed;
    }
    
    if (button_pressed && !button_was_pressed) {
        // Button just pressed - verify it's still pressed after debounce
        delay(BUTTON_DEBOUNCE_TIME);
        if (digitalRead(RESET_BUTTON_PIN) == LOW) {
            button_press_start = now;
            button_was_pressed = true;
            ESP_LOGI(TAG, "Reset button pressed - hold for %d seconds to reset WiFi", RESET_HOLD_TIME / 1000);
        }
    } else if (button_pressed && button_was_pressed) {
        // Button is being held
        unsigned long hold_time = now - button_press_start;
        
        if (hold_time >= RESET_HOLD_TIME) {
            // Button held long enough - reset WiFi credentials
            ESP_LOGW(TAG, "!!! RESETTING WiFi credentials !!!");
            strcpy(ip_display_text, "RESET!");
            led_matrix_show_text("RESET");
            delay(1000);
            
            wifi_prov_reset();
            
            ESP_LOGI(TAG, "WiFi credentials cleared. Restarting...");
            strcpy(ip_display_text, "Restart");
            led_matrix_show_text("RESTART");
            delay(2000);
            
            ESP.restart();
        }
    } else if (!button_pressed && button_was_pressed) {
        // Button released
        unsigned long hold_time = now - button_press_start;
        if (hold_time < RESET_HOLD_TIME) {
            ESP_LOGI(TAG, "Reset button released (held for %lu ms - not long enough)", hold_time);
        }
        button_was_pressed = false;
    }
    
    // Read potentiometer and update volume
    if (now - last_volume_read >= VOLUME_READ_INTERVAL) {
        last_volume_read = now;
        
        // Read ADC value (0-4095 for 12-bit ADC)
        int adc_value = analogRead(VOLUME_POT_PIN);
        
        // Map to 0-100 range
        int volume_percent = map(adc_value, 0, 4095, 0, 100);
        
        // Only update if changed significantly (avoid jitter)
        if (abs(volume_percent - last_volume_value) >= 2) {
            last_volume_value = volume_percent;
            
            // Convert to 0.0-1.0 range for audio library
            float volume_float = volume_percent / 100.0;
            
            // Set the volume
            audio_set_volume(volume_float);
            
            ESP_LOGI(TAG, "Volume potentiometer: ADC=%d, Volume=%d%% (%.2f)", 
                     adc_value, volume_percent, volume_float);
        }
    }
    
    // Check WiFi status periodically and update IP if connected
    if (wifi_connected && (now - last_status_check > 500)) {
        last_status_check = now;
        
        // Get IP from ESP-IDF directly (not Arduino WiFi library)
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif != NULL) {
            esp_netif_ip_info_t ip_info;
            if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
                uint32_t ip = ip_info.ip.addr;
                
                if (ip != 0) {
                    // Valid IP address - update display
                    char new_ip[50];
                    snprintf(new_ip, sizeof(new_ip), 
                             "IP: " IPSTR, IP2STR(&ip_info.ip));
                    
                    // Only log if IP changed
                    if (strcmp(new_ip, ip_display_text) != 0) {
                        strcpy(ip_display_text, new_ip);
                        ESP_LOGI(TAG, "IP Address updated: %s", ip_display_text);
                    }
                }
            }
        }
    }
    
    // Continuously scroll the current status text
    uint16_t color;
    
    if (wifi_connected) {
        color = 0x07E0; // Green (RGB565: 0b0000011111100000)
    } else if (wifi_connecting) {
        color = 0x07FF; // Cyan for connecting
    } else {
        color = 0xFD20; // Orange (RGB565: 0b1111110100100000)
    }
    
    led_matrix_scroll_text(ip_display_text, color);
    
    // Handle audio playback
    audio_loop();
    
    // Play connected sound first
    if (play_connected_sound && !audio_is_playing()) {
        ESP_LOGI(TAG, "Playing connected sound...");
        audio_play_mp3_file("/connected.mp3");
        play_connected_sound = false;
        download_test_file = true;  // Set flag to download test file after connected sound
    }
    
    // Start download 5 seconds after WiFi connected (non-blocking)
    if (download_test_file && !audio_is_playing() && !file_downloaded && !download_started && (now - wifi_connected_time > 5000)) {
        ESP_LOGI(TAG, "Starting MP3 download (non-blocking)...");
        
        // Check available space
        size_t total = LittleFS.totalBytes();
        size_t used = LittleFS.usedBytes();
        ESP_LOGI(TAG, "LittleFS: %d KB total, %d KB used, %d KB free", 
                 total/1024, used/1024, (total-used)/1024);
        
        // Start non-blocking download
        if (audio_start_download("https://mdnwjeafufvrrxphotzv.supabase.co/storage/v1/object/public/feynman/dubai_pirate.mp3")) {
            download_started = true;
            strcpy(ip_display_text, "Downloading...");
        } else {
            ESP_LOGE(TAG, "Failed to start download!");
            download_test_file = false;
        }
    }
    
    // Check if download completed
    if (download_started && audio_download_complete()) {
        file_downloaded = true;
        file_ready_to_play = true;
        download_started = false;
        download_test_file = false;
        audio_reset_download_state();
        ESP_LOGI(TAG, "File downloaded successfully! Press button on GPIO 33 to play.");
        // Restore IP display
        strcpy(ip_display_text, "Ready to play");
    } else if (download_started && audio_download_failed()) {
        ESP_LOGE(TAG, "File download failed!");
        download_started = false;
        download_test_file = false;
        audio_reset_download_state();
        strcpy(ip_display_text, "Download failed");
    }
    
    // Check play button (GPIO 33) - active LOW with pause/stop functionality
    bool play_button_state = digitalRead(PLAY_BUTTON_PIN);
    
    if (play_button_state == LOW && !play_button_was_pressed) {
        // Button just pressed
        delay(50);  // Debounce
        if (digitalRead(PLAY_BUTTON_PIN) == LOW) {
            play_button_press_start = now;
            play_button_was_pressed = true;
            ESP_LOGI(TAG, "Play button pressed");
        }
    } else if (play_button_state == LOW && play_button_was_pressed) {
        // Button is being held - check for stop (4 seconds)
        unsigned long hold_time = now - play_button_press_start;
        
        if (hold_time >= PLAY_BUTTON_STOP_HOLD_TIME && audio_is_playing()) {
            ESP_LOGI(TAG, "Stopping playback (held for %lu ms)", hold_time);
            audio_stop();
            file_ready_to_play = true;  // Allow replay
            play_button_was_pressed = false;  // Prevent repeated triggers
        }
    } else if (play_button_state == HIGH && play_button_was_pressed) {
        // Button released - check what action to take
        unsigned long hold_time = now - play_button_press_start;
        play_button_was_pressed = false;
        
        // Only act on short press (not if we already stopped)
        if (hold_time < PLAY_BUTTON_STOP_HOLD_TIME) {
            if (audio_is_playing()) {
                // Toggle pause/resume
                audio_toggle_pause();
                if (audio_is_paused()) {
                    ESP_LOGI(TAG, "Playback paused");
                } else {
                    ESP_LOGI(TAG, "Playback resumed");
                }
            } else if (file_ready_to_play) {
                // Start playback
                ESP_LOGI(TAG, "Starting playback...");
                if (audio_play_mp3_file("/temp_stream.mp3")) {
                    file_ready_to_play = false;  // Mark as playing
                    ESP_LOGI(TAG, "Playback started!");
                } else {
                    ESP_LOGE(TAG, "Failed to start playback!");
                }
            } else {
                ESP_LOGI(TAG, "No file ready to play!");
            }
        }
    }
    play_button_last_state = play_button_state;
    
    // Play ready sound every 10 seconds when BLE is ready and not connected
    if (ble_ready && !wifi_connected && (now - last_ready_sound >= READY_SOUND_INTERVAL)) {
        ESP_LOGI(TAG, "Playing ready sound...");
        // Play MP3 file from LittleFS
        audio_play_mp3_file("/ready.mp3");
        last_ready_sound = now;
    }
    
    delay(10); // Small delay to prevent tight loop
}
