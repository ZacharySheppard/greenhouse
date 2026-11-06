#include "wifi.hpp"
#include <algorithm>

namespace {

EventGroupHandle_t s_wifi_event_group = nullptr;
auto s_retry_num = 0;
void connect(void *arg, esp_event_base_t event_base, int32_t event_id,
             void *event_data) {
  esp_wifi_connect();
}

void reconnect(void *arg, esp_event_base_t event_base, int32_t event_id,
               void *event_data) {
  if (s_retry_num < 3) {
    esp_wifi_connect();
    s_retry_num++;
    ESP_LOGI(TAG, "retry to connect to the AP");
    return;
  }
  xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
  ESP_LOGI(TAG, "connect to the AP fail");
}

void register_ip(void *arg, esp_event_base_t event_base, int32_t event_id,
                 void *event_data) {

  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
  ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
  s_retry_num = 0;
  xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
}

} // namespace
WifiStation::WifiResult WifiStation::create(WifiStationConfig config) {
  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  s_wifi_event_group = xEventGroupCreate();
  if (s_wifi_event_group == nullptr) {
    return std::unexpected(StationCreationError::unkown);
  }
  s_retry_num = 0;
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  WifiEventHandler()
      .register_event(WIFI_EVENT_STA_START, connect)
      .register_event(WIFI_EVENT_STA_DISCONNECTED, reconnect);

  IPEventHandler().register_event(IP_EVENT_STA_GOT_IP, register_ip);

  wifi_config_t wifi_config = {};
  std::memcpy(wifi_config.sta.ssid, config.ssid.c_str(),
              std::min(config.ssid.size(), sizeof(wifi_config.sta.ssid)));
  std::memcpy(wifi_config.sta.password, config.password.c_str(),
              std::min(config.password.size(), sizeof(wifi_config.sta.password)));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "wifi_init_sta finished.");

  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE, pdFALSE, portMAX_DELAY);

  if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG, "Failed to connect to SSID:%s", config.ssid.c_str());
    return std::unexpected(StationCreationError::unauthorized);
  }
  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "connected to ap SSID:%s", config.ssid.c_str());
    return WifiStation();
  }
  ESP_LOGE(TAG, "UNEXPECTED EVENT");
  return std::unexpected(StationCreationError::unkown);
}
