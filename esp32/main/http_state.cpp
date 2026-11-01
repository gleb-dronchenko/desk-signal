#include "http_state.hpp"
#include "lock.hpp"
#include "wifi_secret.h"

#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include <cstdio>

struct StatePair
{
    Reading *reading;
    SemaphoreHandle_t mutex;
};

static StatePair pair_ = {};
static httpd_handle_t server_ = nullptr;

static Reading *reading_ = nullptr;
static SemaphoreHandle_t mutex_ = nullptr;
static EventGroupHandle_t events_ = nullptr;

// event-group bit set when the station receives an IP address
static constexpr EventBits_t GOT_IP_BIT = BIT0;

// copy the source string to the destination buffer without '\0'
static void CopyField(uint8_t *dst, size_t dst_size, const char *src)
{
    size_t i = 0;

    while (src[i] != '\0' && i < dst_size)
    {
        // copy the character to the destination buffer (uint8_t)
        dst[i] = static_cast<uint8_t>(src[i]);

        ++i;
    }
}

// event handler for the network events
extern "C" void NetworkEvent(void *, esp_event_base_t base, int32_t id, void *)
{
    // if the Wi-Fi is starting, connect to the Wi-Fi
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START)
        esp_wifi_connect();
    // if the Wi-Fi is disconnected, connect to the Wi-Fi
    else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED)
        esp_wifi_connect();
    // if the IP address is received, set the event group bit
    else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP)
        xEventGroupSetBits(events_, GOT_IP_BIT);
}

static void InitNvs()
{
    // initialize the non-volatile storage
    esp_err_t nvs = nvs_flash_init();

    // if the non-volatile storage is not initialized, erase it and initialize it again
    if (nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // erase the non-volatile storage
        nvs_flash_erase();

        // initialize the non-volatile storage again
        nvs_flash_init();
    }
}

static esp_netif_t *InitStation()
{
    // initialize the network interface
    esp_netif_init();

    // create the event loop
    esp_event_loop_create_default();

    // create the station interface
    esp_netif_t *sta = esp_netif_create_default_wifi_sta();

    // initialize the Wi-Fi configuration
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();

    // initialize the Wi-Fi
    esp_wifi_init(&init);

    // set the Wi-Fi mode to station
    esp_wifi_set_mode(WIFI_MODE_STA);

    return sta;
}

static void StartStation()
{
    wifi_config_t wifi_config = {};

    // copy the SSID to the destination buffer
    CopyField(wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), wifi_ssid);

    // copy the password to the destination buffer
    CopyField(wifi_config.sta.password, sizeof(wifi_config.sta.password), wifi_password);
    
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    events_ = xEventGroupCreate();

    // register the event handler for the Wi-Fi events
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &NetworkEvent, nullptr);

    // register the event handler for the IP events
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &NetworkEvent, nullptr);
    
    esp_wifi_start();
}

static void ReadState(const StatePair *pair, uint8_t *mask, bool *fresh)
{
    Lock lock(pair->mutex);
    pair->reading->Expire(esp_timer_get_time());
    pair->reading->Copy(mask, fresh);
}

static const char *JsonBool(bool value)
{
    return value ? "true" : "false";
}

static int WriteState(char *body, size_t size, uint8_t mask, bool fresh)
{
    return snprintf(
        body,
        size,
        "{\"forward\":%s,\"back\":%s,\"left\":%s,\"right\":%s,\"fresh\":%s}",
        JsonBool((mask & 1) != 0),
        JsonBool((mask & 2) != 0),
        JsonBool((mask & 4) != 0),
        JsonBool((mask & 8) != 0),
        JsonBool(fresh));
}

extern "C" esp_err_t StateGet(httpd_req_t *req)
{
    StatePair *pair = static_cast<StatePair *>(req->user_ctx);

    uint8_t mask = 0;
    bool fresh = false;
    ReadState(pair, &mask, &fresh);

    char body[80];
    int length = WriteState(body, sizeof(body), mask, fresh);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, body, length);
    return ESP_OK;
}

static void StartListener()
{
    if (server_ != nullptr)
        return;

    pair_.reading = reading_;
    pair_.mutex = mutex_;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server_, &config);

    httpd_uri_t uri = {};
    uri.uri = "/state";
    uri.method = HTTP_GET;
    uri.handler = StateGet;
    uri.user_ctx = &pair_;
    httpd_register_uri_handler(server_, &uri);
}

extern "C" void NetworkTask(void *)
{
    // initialize the non-volatile storage
    InitNvs();

    esp_netif_t *sta = InitStation();
    StartStation();

    // loop forever until the IP address is received
    for (;;)
    {
        // wait for the IP address to be received
        xEventGroupWaitBits(events_, GOT_IP_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

        esp_netif_ip_info_t info = {};

        // get the IP address
        esp_netif_get_ip_info(sta, &info);
        
        // log the IP address
        ESP_LOGI("network", IPSTR, IP2STR(&info.ip));
        
        StartListener();
    }
}

void StartNetwork(Reading *reading, SemaphoreHandle_t mutex)
{
    reading_ = reading;
    mutex_ = mutex;

    // create the network task
    xTaskCreate(NetworkTask, "network", 8192, nullptr, 5, nullptr);
}