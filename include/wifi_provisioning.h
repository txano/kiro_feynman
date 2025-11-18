#ifndef WIFI_PROVISIONING_H
#define WIFI_PROVISIONING_H

#include <stdbool.h>

// Callback function types
typedef void (*wifi_prov_status_cb_t)(const char* status);

// Initialize WiFi provisioning
void wifi_prov_init(wifi_prov_status_cb_t status_callback);

// Start BLE provisioning
void wifi_prov_start();

// Check if device is provisioned
bool wifi_prov_is_provisioned();

// Reset provisioning (for testing)
void wifi_prov_reset();

#endif // WIFI_PROVISIONING_H
