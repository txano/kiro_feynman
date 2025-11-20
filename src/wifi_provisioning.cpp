#include "wifi_provisioning.h"
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_softap.h>

static const char *TAG = "wifi_prov";

#define PROV_QR_VERSION         "v1"
#define PROV_TRANSPORT_SOFTAP   "softap"
#define QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"

// Event group
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_EVENT BIT0

static wifi_prov_status_cb_t status_cb = NULL;

// Event handler for WiFi and provisioning events
static void event_handler(void* arg, esp_event_base_t event_base,
                         int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Event: base=%s, id=%d", event_base, event_id);
    
    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
            case WIFI_PROV_START:
                ESP_LOGI(TAG, "WIFI_PROV_START - Provisioning started");
                if (status_cb) status_cb("AP_STARTED");
                break;
            case WIFI_PROV_CRED_RECV: {
                wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
                ESP_LOGI(TAG, "WIFI_PROV_CRED_RECV - Received Wi-Fi credentials - SSID:%s", (const char *) wifi_sta_cfg->ssid);
                if (status_cb) status_cb("WIFI_CONNECTING");
                break;
            }
            case WIFI_PROV_CRED_FAIL: {
                wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
                ESP_LOGE(TAG, "WIFI_PROV_CRED_FAIL - Provisioning failed! Reason : %s",
                         (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Wi-Fi auth error" : "Wi-Fi AP not found");
                if (status_cb) status_cb("ERROR");
                break;
            }
            case WIFI_PROV_CRED_SUCCESS:
                ESP_LOGI(TAG, "WIFI_PROV_CRED_SUCCESS - Provisioning successful, waiting for IP...");
                // Don't send callback here - wait for IP_EVENT_STA_GOT_IP instead
                break;
            case WIFI_PROV_END:
                ESP_LOGI(TAG, "WIFI_PROV_END - Provisioning end");
                wifi_prov_mgr_deinit();
                break;
            default:
                ESP_LOGW(TAG, "Unhandled WIFI_PROV_EVENT: %d", event_id);
                break;
        }
    } else if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WIFI_EVENT_STA_START - Connecting to WiFi...");
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED: {
                wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*) event_data;
                ESP_LOGE(TAG, "WIFI_EVENT_STA_DISCONNECTED - Reason: %d", disconnected->reason);
                
                // Reason codes:
                // 2 = AUTH_EXPIRE, 15 = 4WAY_HANDSHAKE_TIMEOUT, 
                // 201 = NO_AP_FOUND, 202 = AUTH_FAIL, 204 = HANDSHAKE_TIMEOUT, 205 = CONNECTION_FAIL
                
                if (disconnected->reason == 202 || disconnected->reason == 15 || disconnected->reason == 204) {
                    ESP_LOGE(TAG, "Authentication failed - likely wrong password!");
                } else if (disconnected->reason == 201) {
                    ESP_LOGE(TAG, "AP not found - check SSID!");
                }
                
                ESP_LOGI(TAG, "Retrying connection...");
                esp_wifi_connect();
                break;
            }
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_START - SoftAP started");
                break;
            case WIFI_EVENT_AP_STACONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED - Station connected to SoftAP");
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED - Station disconnected from SoftAP");
                break;
            default:
                ESP_LOGD(TAG, "Unhandled WIFI_EVENT: %d", event_id);
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP - Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "IP: %d.%d.%d.%d", 
                 IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_EVENT);
        if (status_cb) status_cb("WIFI_CONNECTED");
    }
}

static void get_device_service_name(char *service_name, size_t max)
{
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "STORY_%02X%02X%02X",
             eth_mac[3], eth_mac[4], eth_mac[5]);
}

void wifi_prov_init(wifi_prov_status_cb_t status_callback)
{
    status_cb = status_callback;
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize TCP/IP
    ESP_ERROR_CHECK(esp_netif_init());

    // Initialize event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_event_group = xEventGroupCreate();

    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    // Initialize Wi-Fi - create both STA and AP interfaces
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();  // Add AP interface for SoftAP provisioning
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_LOGI(TAG, "WiFi provisioning initialized (STA + AP interfaces created)");
}

void wifi_prov_start()
{
    ESP_LOGI(TAG, "=== wifi_prov_start() called ===");
    
    // Configuration for provisioning manager using SoftAP
    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_softap,
        .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE
    };

    ESP_LOGI(TAG, "Initializing provisioning manager...");
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));
    ESP_LOGI(TAG, "Device provisioned status: %s", provisioned ? "YES" : "NO");

    if (!provisioned) {
        ESP_LOGI(TAG, "Starting NEW provisioning session");

        char service_name[12];
        get_device_service_name(service_name, sizeof(service_name));
        ESP_LOGI(TAG, "Generated service name: %s", service_name);

        // SoftAP provisioning configuration
        // Using SECURITY_1 with PoP for ESP SoftAP Prov app compatibility
        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
        const char *pop = "abcd1234"; // Proof of possession - enter this in the app
        const char *service_key = NULL; // Password for the SoftAP (NULL = open network)

        ESP_LOGI(TAG, "Security: SECURITY_1 (with PoP)");
        ESP_LOGI(TAG, "Proof of Possession (PoP): %s", pop ? pop : "NONE");
        ESP_LOGI(TAG, "Service key (password): %s", service_key ? service_key : "NONE (open network)");
        
        esp_err_t err = wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start provisioning: %s", esp_err_to_name(err));
            return;
        }

        ESP_LOGI(TAG, "✓ Provisioning started successfully!");
        ESP_LOGI(TAG, "✓ AP name: %s", service_name);
        ESP_LOGI(TAG, "✓ Connect to this AP and use ESP SoftAP Prov app");
        ESP_LOGI(TAG, "✓ Or browse to http://192.168.4.1:80/prov/");

        if (status_cb) status_cb("AP_STARTED");
    } else {
        ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA mode");
        
        wifi_prov_mgr_deinit();
        
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
        
        if (status_cb) status_cb("WIFI_CONNECTING");
    }
    
    ESP_LOGI(TAG, "=== wifi_prov_start() completed ===");
}

bool wifi_prov_is_provisioned()
{
    bool provisioned = false;
    if (wifi_prov_mgr_is_provisioned(&provisioned) == ESP_OK) {
        return provisioned;
    }
    return false;
}

void wifi_prov_reset()
{
    ESP_LOGI(TAG, "Resetting provisioning");
    wifi_prov_mgr_reset_provisioning();
}
