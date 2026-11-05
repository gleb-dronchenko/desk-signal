#include "http_state.hpp"
#include "lock.hpp"
#include "state_body.hpp"
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

static constexpr EventBits_t kGotIp = BIT0;

// ssid and password are fixed 32-byte fields. A full value has no room for a terminator.
static void CopyField(uint8_t *dst, size_t dst_size, const char *src)
{
    size_t i = 0;

    while (src[i] != '\0' && i < dst_size)
    {
        dst[i] = static_cast<uint8_t>(src[i]);
        ++i;
    }
}

extern "C" void NetworkEvent(void *arg, esp_event_base_t base, int32_t id, void *)
{
    EventGroupHandle_t events = static_cast<EventGroupHandle_t>(arg);

    if (base == WIFI_EVENT && (id == WIFI_EVENT_STA_START || id == WIFI_EVENT_STA_DISCONNECTED))
        esp_wifi_connect();
    else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP)
        xEventGroupSetBits(events, kGotIp);
}

static void InitNvs()
{
    esp_err_t nvs = nvs_flash_init();

    if (nvs == ESP_ERR_NVS_NO_FREE_PAGES || nvs == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }
}

static esp_netif_t *InitStation()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *sta = esp_netif_create_default_wifi_sta();
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&init);
    esp_wifi_set_mode(WIFI_MODE_STA);
    return sta;
}

static void StartStation(EventGroupHandle_t events)
{
    wifi_config_t wifi_config = {};
    CopyField(wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), wifi_ssid);
    CopyField(wifi_config.sta.password, sizeof(wifi_config.sta.password), wifi_password);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &NetworkEvent, events);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &NetworkEvent, events);
    esp_wifi_start();
}

static void ReadState(SharedReading *shared, uint8_t *mask, bool *fresh)
{
    Lock lock(shared->mutex);
    shared->reading->Expire(esp_timer_get_time());
    shared->reading->Copy(mask, fresh);
}

extern "C" esp_err_t StateGet(httpd_req_t *req)
{
    SharedReading *shared = static_cast<SharedReading *>(req->user_ctx);

    uint8_t mask = 0;
    bool fresh = false;
    ReadState(shared, &mask, &fresh);

    char body[80];
    int length = WriteStateBody(body, sizeof(body), mask, fresh);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, body, length);
    return ESP_OK;
}

static void StartListener(SharedReading *shared, httpd_handle_t *server)
{
    if (*server != nullptr)
        return;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(server, &config);

    httpd_uri_t uri = {};
    uri.uri = "/state";
    uri.method = HTTP_GET;
    uri.handler = StateGet;
    uri.user_ctx = shared;
    httpd_register_uri_handler(*server, &uri);
}

extern "C" void NetworkTask(void *arg)
{
    SharedReading *shared = static_cast<SharedReading *>(arg);

    InitNvs();
    esp_netif_t *sta = InitStation();
    EventGroupHandle_t events = xEventGroupCreate();
    StartStation(events);

    httpd_handle_t server = nullptr;

    for (;;)
    {
        xEventGroupWaitBits(events, kGotIp, pdTRUE, pdFALSE, portMAX_DELAY);

        esp_netif_ip_info_t info = {};
        esp_netif_get_ip_info(sta, &info);
        ESP_LOGI("network", IPSTR, IP2STR(&info.ip));
        StartListener(shared, &server);
    }
}

void StartNetwork(SharedReading *shared)
{
    xTaskCreate(NetworkTask, "network", 8192, shared, 5, nullptr);
}
