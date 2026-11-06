#include "requests.hpp"
#include "config.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi.hpp"
#include <array>
#include <cstdio>
#include <string_view>
#include <utility>
namespace {
constexpr std::string_view port = "443";
constexpr auto key = find_value("API_SECRET");

void request(std::string_view verb, std::string endpoint,
             std::string_view data = {}) {
  constexpr std::string_view scheme = "https://";
  if (endpoint.starts_with(scheme)) {
    endpoint.erase(0, scheme.size());
  }
  const auto slash = endpoint.find('/');
  const std::string host = endpoint.substr(0, slash);
  const std::string path =
      slash == std::string::npos ? "/" : endpoint.substr(slash);
  if (host.empty()) {
    ESP_LOGE(TAG, "HTTPS endpoint has no host");
    return;
  }

  auto connection = Socket::connect_to({host, std::string(port)});
  if (!connection) {
    return;
  }

  std::string message;
  message.reserve(verb.size() + path.size() + host.size() + data.size() + 256);
  message += verb;
  message += ' ';
  message += path;
  message += " HTTP/1.0\r\nHost: ";
  message += host;
  message += "\r\nUser-Agent: esp-idf/1.0 esp32\r\nConnection: close\r\n";
  message += "x-api-key: ";
  message += *key;
  message += "\r\n";
  if (verb == "POST") {
    message += "Content-Type: application/json\r\nContent-Length: ";
    message += std::to_string(data.size());
    message += "\r\n";
  }
  message += "\r\n";
  message += data;

  if (!connection->Write(message)) {
    return;
  }

  std::array<std::byte, 1024> buffer;
  int retries = 0;
  while (true) {
    auto result = connection->Read(buffer);
    if (!result) {
      if (result.error() == ReadError::again && retries++ < 5) {
        vTaskDelay(pdMS_TO_TICKS(10));
        continue;
      }
      break;
    }
    retries = 0;
    if (*result == 0) {
      break;
    }
    for (size_t i = 0; i < *result; ++i) {
      putchar(static_cast<char>(buffer[i]));
    }
  }
  putchar('\n');
}
} // namespace

void get(std::string endpoint) { request("GET", std::move(endpoint)); }

void post(std::string endpoint, std::string data) {
  request("POST", std::move(endpoint), data);
}
