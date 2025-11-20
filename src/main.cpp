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
    
    // Initialize WiFi provisioning
    ESP_LOGI(TAG, "Initializing WiFi provisioning...");
    wifi_prov_init(wifi_status_callback);
    
    // TEMPORARY: Reset WiFi credentials for testing
    // Comment this out after testing!
    ESP_LOGI(TAG, "!!! RESETTING WiFi credentials for testing !!!");
    wifi_prov_reset();
    delay(1000); // Give it time to clear NVS
    
    // Start provisioning
    ESP_LOGI(TAG, "Starting WiFi provisioning...");
    wifi_prov_start();
    
    ESP_LOGI(TAG, "Setup complete!");
}

void loop() {
    // Check WiFi status periodically and update IP if connected
    unsigned long now = millis();
    if (wifi_connected && (now - last_status_check > 500)) {
        last_status_check = now;
        
        // Get IP from ESP-IDF directly (not Arduino WiFi library)
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif != NULL) {
            esp_netif_ip_info_t ip_info;
            if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
                uint32_t ip = ip_info.ip.addr;
                
                ESP_LOGI(TAG, "Checking IP: " IPSTR " (wifi_connected=%d)", 
                         IP2STR(&ip_info.ip), wifi_connected);
                
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
                } else {
                    ESP_LOGW(TAG, "WiFi connected but no IP yet!");
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
