#include "openai_tts.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <esp_log.h>
#include <esp_http_client.h>
#include <esp_tls.h>
#include <lwip/dns.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include "audio.h"

static const char *TAG = "openai_tts";
static String api_key = "";

// OpenAI TTS API endpoint
#define OPENAI_TTS_URL "https://api.openai.com/v1/audio/speech"

void openai_tts_init(const char* key) {
    api_key = String(key);
    ESP_LOGI(TAG, "OpenAI TTS initialized");
}

void openai_tts_test_dns() {
    ESP_LOGI(TAG, "=== DNS Resolution Test (ESP-IDF) ===");
    
    // Test 1: Try to resolve google.com using lwIP
    ESP_LOGI(TAG, "Resolving google.com using lwIP...");
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    int err = getaddrinfo("google.com", "80", &hints, &res);
    if (err == 0 && res != NULL) {
        struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr->sin_addr, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "✓ google.com resolved to: %s", ip_str);
        freeaddrinfo(res);
    } else {
        ESP_LOGE(TAG, "✗ Failed to resolve google.com (error: %d)", err);
    }
    
    // Test 2: Try to resolve api.openai.com
    ESP_LOGI(TAG, "Resolving api.openai.com...");
    err = getaddrinfo("api.openai.com", "443", &hints, &res);
    if (err == 0 && res != NULL) {
        struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr->sin_addr, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "✓ api.openai.com resolved to: %s", ip_str);
        freeaddrinfo(res);
    } else {
        ESP_LOGE(TAG, "✗ Failed to resolve api.openai.com (error: %d)", err);
    }
    
    // Test 3: Try HTTP connection to google.com using sockets
    ESP_LOGI(TAG, "Testing HTTP connection to google.com...");
    err = getaddrinfo("google.com", "80", &hints, &res);
    if (err == 0 && res != NULL) {
        int sock = socket(res->ai_family, res->ai_socktype, 0);
        if (sock >= 0) {
            struct timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            
            if (connect(sock, res->ai_addr, res->ai_addrlen) == 0) {
                ESP_LOGI(TAG, "✓ Connected to google.com:80");
                close(sock);
            } else {
                ESP_LOGE(TAG, "✗ Failed to connect to google.com:80");
                close(sock);
            }
        }
        freeaddrinfo(res);
    }
    
    ESP_LOGI(TAG, "=== DNS Test Complete ===");
}

// HTTP event handler for streaming response to file
static File tts_file;
static int total_bytes_received = 0;

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (tts_file) {
                tts_file.write((uint8_t*)evt->data, evt->data_len);
                total_bytes_received += evt->data_len;
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

bool openai_tts_speak(const char* text) {
    if (api_key.length() == 0) {
        ESP_LOGE(TAG, "API key not set");
        return false;
    }
    
    ESP_LOGI(TAG, "Requesting TTS for: %s", text);
    
    // Create JSON request body
    StaticJsonDocument<256> doc;
    doc["model"] = "tts-1";
    doc["input"] = text;
    doc["voice"] = "alloy";
    doc["response_format"] = "mp3";
    
    String requestBody;
    serializeJson(doc, requestBody);
    
    // Open file for writing
    tts_file = LittleFS.open("/temp_tts.mp3", "w");
    if (!tts_file) {
        ESP_LOGE(TAG, "Failed to open temp file");
        return false;
    }
    total_bytes_received = 0;
    
    // Configure ESP-IDF HTTP client with TLS
    esp_http_client_config_t config = {};
    config.url = OPENAI_TTS_URL;
    config.method = HTTP_METHOD_POST;
    config.event_handler = http_event_handler;
    config.timeout_ms = 30000;
    config.buffer_size = 2048;
    config.buffer_size_tx = 2048;
    config.skip_cert_common_name_check = true;  // Skip certificate verification for simplicity
    config.transport_type = HTTP_TRANSPORT_OVER_SSL;
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    // Set headers
    String auth_header = "Bearer " + api_key;
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", auth_header.c_str());
    esp_http_client_set_post_field(client, requestBody.c_str(), requestBody.length());
    
    ESP_LOGI(TAG, "Sending request to OpenAI...");
    esp_err_t err = esp_http_client_perform(client);
    
    tts_file.close();
    
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP Status: %d, Bytes received: %d", status_code, total_bytes_received);
        
        if (status_code == 200 && total_bytes_received > 0) {
            esp_http_client_cleanup(client);
            ESP_LOGI(TAG, "TTS audio saved, playing...");
            return audio_play_mp3_file("/temp_tts.mp3");
        } else {
            ESP_LOGE(TAG, "HTTP request failed with status: %d", status_code);
            esp_http_client_cleanup(client);
            return false;
        }
    } else {
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return false;
    }
}
