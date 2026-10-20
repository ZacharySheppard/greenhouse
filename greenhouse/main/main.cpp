#include "Device.hpp"
extern "C" void app_main(void)
{
    const auto device = Device();
    device.run();
}