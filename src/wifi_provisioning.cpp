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
#include <wifi_provisioning/scheme_ble.h>

static const char *TAG = "wifi_prov";

#define PROV_QR_VERSION         "v1"
#define PROV_TRANSPORT_BLE      "ble"
#define QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"

// Event group
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_EVENT BIT0

static wifi_prov_status_cb_t status_cb = NULL;

// Event handler for WiFi and provisioning events
static void event_handler(void* arg, esp_event_base_t event_base,
                         int32_t event_id, void* event_data)
{
    if (event_base == WIFI_PROV_EVENT) {
        switch (event_id) {
            case WIFI_PROV_START:
                ESP_LOGI(TAG, "Provisioning started");
                if (status_cb) status_cb("BLE_SEARCHING");
                break;
            case WIFI_PROV_CRED_RECV: {
                wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
                ESP_LOGI(TAG, "Received Wi-Fi credentials - SSID:%s", (const char *) wifi_sta_cfg->ssid);
                if (status_cb) status_cb("BLE_CONNECTED");
                break;
            }
            case WIFI_PROV_CRED_FAIL: {
                wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
                ESP_LOGE(TAG, "Provisioning failed! Reason : %s",
                         (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Wi-Fi auth error" : "Wi-Fi AP not found");
                if (status_cb) status_cb("ERROR");
                break;
            }
            case WIFI_PROV_CRED_SUCCESS:
                ESP_LOGI(TAG, "Provisioning successful");
                if (status_cb) status_cb("WIFI_CONNECTING");
                break;
            case WIFI_PROV_END:
                ESP_LOGI(TAG, "Provisioning end");
                wifi_prov_mgr_deinit();
                break;
            default:
                break;
        }
    } else if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
                esp_wifi_connect();
                break;
            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
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

    // Initialize Wi-Fi
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_LOGI(TAG, "WiFi provisioning initialized");
}

void wifi_prov_start()
{
    // Configuration for provisioning manager
    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_ble,
        .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
    };

    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));

    if (!provisioned) {
        ESP_LOGI(TAG, "Starting provisioning");

        char service_name[12];
        get_device_service_name(service_name, sizeof(service_name));

        // Generate QR code for easy provisioning
        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
        const char *pop = "abcd1234"; // Proof of possession
        const char *service_key = NULL;

        uint8_t custom_service_uuid[] = {
            0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
            0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
        };

        wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);

        ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));

        ESP_LOGI(TAG, "Provisioning started with service name: %s", service_name);
        ESP_LOGI(TAG, "Scan QR code or use ESP BLE Prov app");
        ESP_LOGI(TAG, "Proof of Possession (PoP): %s", pop);

        if (status_cb) status_cb("BLE_SEARCHING");
    } else {
        ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");
        
        wifi_prov_mgr_deinit();
        
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
        
        if (status_cb) status_cb("WIFI_CONNECTING");
    }
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
