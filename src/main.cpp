#include <Arduino.h>
#include <string.h>
#include "led_matrix.h"
#include "wifi_provisioning.h"
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <lwip/ip4_addr.h>

static const char *TAG = "main";
static bool wifi_connected = false;
static bool wifi_connecting = false;
static char ip_display_text[50] = "No WiFi";
static unsigned long last_status_check = 0;

// Reset button configuration (BOOT button on ESP32-S3)
#define RESET_BUTTON_PIN 0
#define RESET_HOLD_TIME 5000  // Hold for 5 seconds to reset WiFi credentials
static unsigned long button_press_start = 0;
static bool button_was_pressed = false;

// Callback for WiFi provisioning status updates
void wifi_status_callback(const char* status) {
    ESP_LOGI(TAG, "WiFi Status: %s", status);
    
    if (strcmp(status, "AP_STARTED") == 0) {
        wifi_connected = false;
        wifi_connecting = false;
        strcpy(ip_display_text, "BLE Ready");
    } else if (strcmp(status, "AP_CONNECTED") == 0) {
        // Just log, don't block
    } else if (strcmp(status, "WIFI_CONNECTING") == 0) {
        wifi_connecting = true;
        wifi_connected = false;
        strcpy(ip_display_text, "Connecting...");
    } else if (strcmp(status, "WIFI_CONNECTED") == 0) {
        wifi_connected = true;
        wifi_connecting = false;
        strcpy(ip_display_text, "Getting IP...");
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
    
    // Initialize reset button (BOOT button)
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    ESP_LOGI(TAG, "Reset button initialized on GPIO %d", RESET_BUTTON_PIN);
    ESP_LOGI(TAG, "Hold BOOT button for %d seconds to reset WiFi credentials", RESET_HOLD_TIME / 1000);
    
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
    
    // Check reset button (BOOT button) - active LOW
    bool button_pressed = (digitalRead(RESET_BUTTON_PIN) == LOW);
    
    if (button_pressed && !button_was_pressed) {
        // Button just pressed
        button_press_start = now;
        button_was_pressed = true;
        ESP_LOGI(TAG, "Reset button pressed - hold for %d seconds to reset WiFi", RESET_HOLD_TIME / 1000);
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
        } else if (hold_time % 1000 < 100) {
            // Show countdown every second
            int seconds_left = (RESET_HOLD_TIME - hold_time) / 1000 + 1;
            ESP_LOGI(TAG, "Hold for %d more seconds...", seconds_left);
        }
    } else if (!button_pressed && button_was_pressed) {
        // Button released
        unsigned long hold_time = now - button_press_start;
        if (hold_time < RESET_HOLD_TIME) {
            ESP_LOGI(TAG, "Reset button released (held for %lu ms - not long enough)", hold_time);
        }
        button_was_pressed = false;
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
    delay(10); // Small delay to prevent tight loop
}
