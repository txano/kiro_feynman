#include <Arduino.h>
#include "led_matrix.h"
#include "wifi_provisioning.h"
#include <esp_log.h>

static const char *TAG = "main";

// Callback for WiFi provisioning status updates
void wifi_status_callback(const char* status) {
    ESP_LOGI(TAG, "WiFi Status: %s", status);
    
    if (strcmp(status, "BLE_SEARCHING") == 0) {
        led_matrix_show_status(STATUS_BLE_SEARCHING);
    } else if (strcmp(status, "BLE_CONNECTED") == 0) {
        led_matrix_show_status(STATUS_BLE_CONNECTED);
    } else if (strcmp(status, "WIFI_CONNECTING") == 0) {
        led_matrix_show_status(STATUS_WIFI_CONNECTING);
    } else if (strcmp(status, "WIFI_CONNECTED") == 0) {
        led_matrix_show_status(STATUS_WIFI_CONNECTED);
        delay(2000);
        led_matrix_show_status(STATUS_READY);
    } else if (strcmp(status, "ERROR") == 0) {
        led_matrix_show_status(STATUS_ERROR);
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
    
    // Start provisioning
    ESP_LOGI(TAG, "Starting WiFi provisioning...");
    wifi_prov_start();
    
    ESP_LOGI(TAG, "Setup complete!");
}

void loop() {
    // Main loop - for now just keep the device running
    // In future iterations, this will handle:
    // - Button inputs
    // - Story selection
    // - Audio streaming
    delay(100);
}
