

#include "driver/gpio.h"

enum class ButtonState
{
    ON,
    OFF
};

class GPIO
{
public:
    [[nodiscard]] static auto as_output(uint8_t pin) noexcept -> GPIO;
    [[nodiscard]] static auto as_input(uint8_t pin) noexcept -> GPIO;

    auto set_on() const noexcept -> void;
    auto set_off() const noexcept -> void;

    [[nodiscard]] auto get_state() const noexcept -> ButtonState;

    [[nodiscard]] auto is_on() const noexcept -> bool;

    [[nodiscard]] auto is_off() const noexcept -> bool;

private:
    explicit GPIO(gpio_config_t config, uint32_t pin) noexcept;

private:
    gpio_config_t config_;
    uint32_t pin_;
};