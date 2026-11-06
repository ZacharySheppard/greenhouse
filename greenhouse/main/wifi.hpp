

#pragma once
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_types.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include <cstdint>
#include <cstring>
#include <expected>
#include <string>
constexpr uint32_t WIFI_CONNECTED_BIT = BIT0;
constexpr uint32_t WIFI_FAIL_BIT = BIT1;

static constexpr const char *TAG = "wifi station";

class WifiEventHandler {
public:
  WifiEventHandler &register_event(int32_t id, esp_event_handler_t callback) {
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, id,
                                                        callback, NULL, &id_));
    return *this;
  }

private:
  esp_event_handler_instance_t id_ = 0;
};

class IPEventHandler {
public:
  IPEventHandler &register_event(int32_t id, esp_event_handler_t callback) {
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, id, callback,
                                                        NULL, &id_));
    return *this;
  }

private:
  esp_event_handler_instance_t id_ = 0;
};

struct WifiStationConfig {
  std::string ssid;
  std::string password;
  int retries = 3;
};

enum class StationCreationError { unauthorized, unkown };

class WifiStation {
  using WifiResult = std::expected<WifiStation, StationCreationError>;

public:
  static WifiResult create(WifiStationConfig config);

protected:
  WifiStation() = default;
};
