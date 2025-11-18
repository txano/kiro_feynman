#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define LED_MATRIX_PIN 14
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

// Status display modes
typedef enum {
    STATUS_BLE_SEARCHING,
    STATUS_BLE_CONNECTED,
    STATUS_WIFI_CONNECTING,
    STATUS_WIFI_CONNECTED,
    STATUS_ERROR,
    STATUS_READY
} led_status_t;

void led_matrix_init();
void led_matrix_show_status(led_status_t status);
void led_matrix_show_text(const char* text);
void led_matrix_clear();
void led_matrix_set_brightness(uint8_t brightness);

#endif // LED_MATRIX_H
