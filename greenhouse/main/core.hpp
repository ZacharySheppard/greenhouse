#include <chrono>
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

template <typename Rep, typename Period>
auto wait_for(std::chrono::duration<Rep, Period> duration) -> void
{

    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    const auto ticks = pdMS_TO_TICKS(ms.count());
    vTaskDelay(ticks);
}