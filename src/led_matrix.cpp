#include "led_matrix.h"
#include <Arduino.h>

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    MATRIX_WIDTH, MATRIX_HEIGHT, LED_MATRIX_PIN,
    NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRB + NEO_KHZ800
);

static int scroll_x = MATRIX_WIDTH;
static int text_width = 0;

void led_matrix_init() {
    matrix.begin();
    matrix.setTextWrap(false);
    matrix.setBrightness(10); // Low brightness to prevent overheating
    matrix.fillScreen(0);
    matrix.show();
}

void led_matrix_set_brightness(uint8_t brightness) {
    matrix.setBrightness(brightness);
}

void led_matrix_clear() {
    matrix.fillScreen(0);
    matrix.show();
}

// Helper to draw simple icons/patterns
static void draw_bluetooth_icon() {
    matrix.fillScreen(0);
    // Simple BT icon pattern
    matrix.drawPixel(3, 1, matrix.Color(0, 0, 255));
    matrix.drawPixel(4, 1, matrix.Color(0, 0, 255));
    matrix.drawPixel(3, 2, matrix.Color(0, 0, 255));
    matrix.drawPixel(4, 2, matrix.Color(0, 0, 255));
    matrix.drawPixel(2, 3, matrix.Color(0, 0, 255));
    matrix.drawPixel(5, 3, matrix.Color(0, 0, 255));
    matrix.drawPixel(3, 4, matrix.Color(0, 0, 255));
    matrix.drawPixel(4, 4, matrix.Color(0, 0, 255));
    matrix.drawPixel(2, 5, matrix.Color(0, 0, 255));
    matrix.drawPixel(5, 5, matrix.Color(0, 0, 255));
    matrix.drawPixel(3, 6, matrix.Color(0, 0, 255));
    matrix.drawPixel(4, 6, matrix.Color(0, 0, 255));
    matrix.show();
}

static void draw_wifi_icon() {
    matrix.fillScreen(0);
    // Simple WiFi icon pattern
    matrix.drawPixel(3, 2, matrix.Color(0, 255, 0));
    matrix.drawPixel(4, 2, matrix.Color(0, 255, 0));
    matrix.drawPixel(2, 3, matrix.Color(0, 255, 0));
    matrix.drawPixel(5, 3, matrix.Color(0, 255, 0));
    matrix.drawPixel(1, 4, matrix.Color(0, 255, 0));
    matrix.drawPixel(6, 4, matrix.Color(0, 255, 0));
    matrix.drawPixel(3, 6, matrix.Color(0, 255, 0));
    matrix.drawPixel(4, 6, matrix.Color(0, 255, 0));
    matrix.show();
}

static void draw_checkmark() {
    matrix.fillScreen(0);
    // Checkmark pattern
    matrix.drawPixel(5, 2, matrix.Color(0, 255, 0));
    matrix.drawPixel(4, 3, matrix.Color(0, 255, 0));
    matrix.drawPixel(5, 3, matrix.Color(0, 255, 0));
    matrix.drawPixel(3, 4, matrix.Color(0, 255, 0));
    matrix.drawPixel(4, 4, matrix.Color(0, 255, 0));
    matrix.drawPixel(2, 5, matrix.Color(0, 255, 0));
    matrix.drawPixel(3, 5, matrix.Color(0, 255, 0));
    matrix.show();
}

static void draw_error() {
    matrix.fillScreen(0);
    // X pattern
    matrix.drawPixel(2, 2, matrix.Color(255, 0, 0));
    matrix.drawPixel(5, 2, matrix.Color(255, 0, 0));
    matrix.drawPixel(3, 3, matrix.Color(255, 0, 0));
    matrix.drawPixel(4, 3, matrix.Color(255, 0, 0));
    matrix.drawPixel(3, 4, matrix.Color(255, 0, 0));
    matrix.drawPixel(4, 4, matrix.Color(255, 0, 0));
    matrix.drawPixel(2, 5, matrix.Color(255, 0, 0));
    matrix.drawPixel(5, 5, matrix.Color(255, 0, 0));
    matrix.show();
}

void led_matrix_show_status(led_status_t status) {
    switch (status) {
        case STATUS_BLE_SEARCHING:
            draw_bluetooth_icon();
            break;
        case STATUS_BLE_CONNECTED:
            draw_bluetooth_icon();
            delay(500);
            draw_checkmark();
            break;
        case STATUS_WIFI_CONNECTING:
            draw_wifi_icon();
            break;
        case STATUS_WIFI_CONNECTED:
            draw_wifi_icon();
            delay(500);
            draw_checkmark();
            break;
        case STATUS_ERROR:
            draw_error();
            break;
        case STATUS_READY:
            draw_checkmark();
            break;
    }
}

void led_matrix_show_text(const char* text) {
    matrix.fillScreen(0);
    matrix.setCursor(scroll_x, 0);
    matrix.setTextColor(matrix.Color(255, 255, 255));
    matrix.print(text);
    matrix.show();
    
    // Update scroll position for next call
    scroll_x--;
    if (scroll_x < -((int)strlen(text) * 6)) {
        scroll_x = MATRIX_WIDTH;
    }
}

void led_matrix_scroll_text(const char* text, uint16_t color) {
    static int local_scroll_x = MATRIX_WIDTH;
    static unsigned long last_update = 0;
    static const char* last_text = nullptr;
    
    // Reset scroll position if text changed
    if (last_text != text) {
        local_scroll_x = MATRIX_WIDTH;
        last_text = text;
    }
    
    // Update every 80ms for smooth scrolling
    unsigned long now = millis();
    if (now - last_update < 80) {
        return;
    }
    last_update = now;
    
    matrix.fillScreen(0);
    matrix.setCursor(local_scroll_x, 0);
    matrix.setTextColor(color);
    matrix.print(text);
    matrix.show();
    
    // Update scroll position
    local_scroll_x--;
    int text_pixel_width = strlen(text) * 6; // Each char is ~6 pixels wide
    if (local_scroll_x < -(text_pixel_width)) {
        local_scroll_x = MATRIX_WIDTH;
    }
}
