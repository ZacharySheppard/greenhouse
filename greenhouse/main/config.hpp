#pragma once

#include <array>
#include <optional>
#include <ranges>
#include <string_view>
#include <utility>

namespace config_detail {
// clang-format off
inline constexpr unsigned char file_bytes[] = {
#embed MY_CONFIG_PATH
};
// clang-format on

inline constexpr auto file_text = [] {
  std::array<char, sizeof(file_bytes)> text{};
  for (std::size_t i = 0; i < text.size(); ++i) {
    text[i] = static_cast<char>(file_bytes[i]);
  }
  return text;
}();

using Entry = std::pair<std::string_view, std::string_view>;

constexpr Entry to_entry(auto line) {
  auto text =
      std::string_view(std::ranges::data(line), std::ranges::size(line));
  if (text.ends_with('\r')) {
    text.remove_suffix(1);
  }
  const auto equals = text.find('=');
  if (equals == std::string_view::npos) {
    return {};
  }
  return {text.substr(0, equals), text.substr(equals + 1)};
}

constexpr auto parsed_entries() {
  const auto text = std::string_view(file_text.data(), file_text.size());
  return text | std::views::split('\n') |
         std::views::transform([](auto line) { return to_entry(line); }) |
         std::views::filter(
             [](const Entry &entry) { return !entry.first.empty(); });
}

inline constexpr auto entry_count = [] {
  std::size_t count = 0;
  for (const auto &entry : parsed_entries()) {
    ++count;
  }
  return count;
}();

inline constexpr auto entries = [] {
  std::array<Entry, entry_count> result{};
  std::size_t index = 0;
  for (const auto &entry : parsed_entries()) {
    result[index++] = entry;
  }
  return result;
}();

} // namespace config_detail

constexpr std::optional<std::string_view> find_value(std::string_view key) {
  for (const auto &[entry_key, value] : config_detail::entries) {
    if (entry_key == key) {
      return value;
    }
  }
  return std::nullopt;
}
