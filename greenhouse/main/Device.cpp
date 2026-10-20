#include "Device.hpp"
#include "gpio.hpp"
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"

#include "core.hpp"
#include "esp_chip_info.h"
#include "esp_flash.h"

namespace
{

    auto get_info() -> esp_chip_info_t
    {
        esp_chip_info_t chip_info;
        esp_chip_info(&chip_info);
        return chip_info;
    }

    auto log_device_info(const esp_chip_info_t &chip_info, const uint32_t flash_size)
    {
        printf("This is %s chip with %d CPU core(s), %s%s%s%s%s%s, ",
               CONFIG_IDF_TARGET,
               chip_info.cores,
               (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi" : "",
               ((chip_info.features & CHIP_FEATURE_WIFI_BGN) && (chip_info.features & (CHIP_FEATURE_BT | CHIP_FEATURE_BLE))) ? "/" : "",
               (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
               ((chip_info.features & CHIP_FEATURE_BT) && (chip_info.features & CHIP_FEATURE_BLE)) ? "/" : "",
               (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
               (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");
        unsigned major_rev = chip_info.revision / 100;
        unsigned minor_rev = chip_info.revision % 100;
        printf("silicon revision v%d.%d, ", major_rev, minor_rev);
        printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
               (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
        printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
    }

    auto get_flash_size() -> uint32_t
    {
        uint32_t flash_size = 0;
        if (esp_flash_get_size(nullptr, &flash_size) != ESP_OK)
        {
            printf("Get flash size failed");
            return 0;
        }
        return flash_size;
    }

} // namespace

Device::Device() noexcept
{
    const auto info = get_info();
    const auto flash_size = get_flash_size();
    log_device_info(info, flash_size);
}

Device::~Device()
{
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

auto Device::run() const -> void
{
    using namespace std::chrono_literals;
    constexpr auto device_should_run = true;

    const std::array pins{
        GPIO::as_output(32),
        GPIO::as_output(33),
        GPIO::as_output(25),
        GPIO::as_output(26),
    };

    while (device_should_run)
    {

        for (const auto pin : pins)
        {
            pin.set_on();
            wait_for(300ms);
            pin.set_off();
        }
    }
}