#include "socket.hpp"
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include "esp_tls_errors.h"
#include "wifi.hpp"
#include <cstdlib>

SocketResult Socket::connect_to(const ConnectionInfo &connection) {
  esp_tls_t *tls = esp_tls_init();
  if (!tls) {
    ESP_LOGE(TAG, "Failed to allocate TLS handle");
    return std::unexpected(SocketError::failed_to_alloc);
  }

  esp_tls_cfg_t cfg = {};
  cfg.crt_bundle_attach = esp_crt_bundle_attach;
  cfg.timeout_ms = 10000;

  const int port = std::atoi(connection.port.c_str());
  if (port <= 0 || port > 65535 ||
      esp_tls_conn_new_sync(connection.address.c_str(),
                            connection.address.size(), port, &cfg, tls) != 1) {
    ESP_LOGE(TAG, "TLS connection to %s:%s failed", connection.address.c_str(),
             connection.port.c_str());
    esp_tls_error_handle_t error_handle = nullptr;
    if (esp_tls_get_error_handle(tls, &error_handle) == ESP_OK && error_handle) {
      int tls_code = 0;
      int verify_flags = 0;
      const esp_err_t tls_error = esp_tls_get_and_clear_last_error(
          error_handle, &tls_code, &verify_flags);
      ESP_LOGE(TAG, "TLS error=%s mbedTLS=%d certificate flags=0x%x",
               esp_err_to_name(tls_error), tls_code, verify_flags);
    }
    esp_tls_conn_destroy(tls);
    return std::unexpected(SocketError::connection_failed);
  }

  ESP_LOGI(TAG, "TLS connection established to %s", connection.address.c_str());
  return Socket(tls);
}

std::expected<void, SocketError> Socket::Write(std::string_view request) {
  size_t written = 0;
  while (written < request.size()) {
    const ssize_t result = esp_tls_conn_write(
        tls_, request.data() + written, request.size() - written);
    if (result > 0) {
      written += static_cast<size_t>(result);
    } else if (result == ESP_TLS_ERR_SSL_WANT_READ ||
               result == ESP_TLS_ERR_SSL_WANT_WRITE) {
      continue;
    } else {
      ESP_LOGE(TAG, "TLS write failed: %d", static_cast<int>(result));
      return std::unexpected(SocketError::write_failed);
    }
  }
  return {};
}

ReadResult Socket::Read(std::span<std::byte> buffer) noexcept {
  const ssize_t result = esp_tls_conn_read(tls_, buffer.data(), buffer.size());
  if (result >= 0) {
    return static_cast<size_t>(result);
  }
  if (result == ESP_TLS_ERR_SSL_WANT_READ ||
      result == ESP_TLS_ERR_SSL_WANT_WRITE) {
    return std::unexpected(ReadError::again);
  }
  ESP_LOGE(TAG, "TLS read failed: %d", static_cast<int>(result));
  return std::unexpected(ReadError::other);
}

void Socket::Close() {
  if (tls_) {
    esp_tls_conn_destroy(tls_);
    tls_ = nullptr;
  }
}
