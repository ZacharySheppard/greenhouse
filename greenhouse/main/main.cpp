#include "config.hpp"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "requests.hpp"
#include "wifi.hpp"
#include <string>

static void report_health(void *) {
  while (true) {
    ESP_LOGI(TAG, "Posting heartbeat");
    post("https://greenhousewebserver.vercel.app/api/heartbeat",
         "{\"deviceId\":\"greenhouse_board_1\"}");
    vTaskDelay(pdMS_TO_TICKS(2 * 60 * 1000));
  }
}

extern "C" void app_main(void) {

  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  const auto config = WifiStationConfig{
      .ssid = std::string(find_value("LOCAL_WIFI_SSID").value_or("none")),
      .password = std::string(find_value("LOCAL_WIFI_PW").value_or("none")),
  };

  const auto station = WifiStation::create(config);
  if (!station) {
    ESP_LOGE(TAG, "Failed to create Wi-Fi station");
    return;
  }

  const esp_sntp_config_t sntp_config =
      ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
  ESP_ERROR_CHECK(esp_netif_sntp_init(&sntp_config));
  while (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK) {
    ESP_LOGW(TAG, "Waiting for clock sync before TLS");
  }

  xTaskCreate(&report_health, "report_health", 8192, nullptr, 5, nullptr);
}
