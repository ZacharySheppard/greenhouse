#include "gpio.hpp"
#include "esp_log.h"
namespace
{
    static const char *TAG = "GPIO_EXAMPLE";
    constexpr auto to_button_state(uint32_t state) -> ButtonState
    {
        return state > 0 ? ButtonState::ON : ButtonState::OFF;
    }
} // namespace

GPIO::GPIO(gpio_config_t config, uint32_t pin) noexcept : config_(config), pin_(pin)
{
    gpio_config(&config_);
}

auto GPIO::as_output(uint8_t pin) noexcept -> GPIO
{
    const auto config = gpio_config_t{
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};

    return GPIO(config, pin);
}

auto GPIO::as_input(uint8_t pin) noexcept -> GPIO
{
    const auto config = gpio_config_t{
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE, // Button connects pin to GND when pressed
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE // Polling approach; no interrupts used here
    };

    return GPIO(config, pin);
}

auto GPIO::set_on() const noexcept -> void
{
    ESP_LOGI(TAG, "Setting pin %i [ON].", pin_);
    gpio_set_level(gpio_num_t(pin_), 1);
};

auto GPIO::set_off() const noexcept -> void
{
    ESP_LOGI(TAG, "Setting pin %i [OFF].", pin_);
    gpio_set_level(gpio_num_t(pin_), 0);
};
auto GPIO::get_state() const noexcept -> ButtonState
{
    return to_button_state(gpio_get_level(gpio_num_t(pin_)));
}
auto GPIO::is_on() const noexcept -> bool
{
    return get_state() == ButtonState::ON;
}
auto GPIO::is_off() const noexcept -> bool
{
    return get_state() == ButtonState::OFF;
}