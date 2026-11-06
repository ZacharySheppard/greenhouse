#pragma once

#include "esp_tls.h"
#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <utility>

struct ConnectionInfo {
  std::string address;
  std::string port;
};

enum class SocketError {
  failed_to_alloc,
  connection_failed,
  write_failed,
};

enum class ReadError { again, other };

class Socket;
using SocketResult = std::expected<Socket, SocketError>;
using ReadResult = std::expected<size_t, ReadError>;

class Socket {
public:
  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;
  Socket(Socket &&other) noexcept : tls_(std::exchange(other.tls_, nullptr)) {}
  Socket &operator=(Socket &&other) noexcept {
    if (this != &other) {
      Close();
      tls_ = std::exchange(other.tls_, nullptr);
    }
    return *this;
  }
  ~Socket() { Close(); }

  static SocketResult connect_to(const ConnectionInfo &connection);
  std::expected<void, SocketError> Write(std::string_view request);
  ReadResult Read(std::span<std::byte> buffer) noexcept;
  void Close();

private:
  explicit Socket(esp_tls_t *tls) : tls_(tls) {}
  esp_tls_t *tls_ = nullptr;
};
