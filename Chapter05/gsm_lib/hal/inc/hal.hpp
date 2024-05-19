#pragma once

#include <cstdint>

#include <stm32f0xx_hal.h>

namespace hal
{
inline void init()
{
    HAL_Init();
}

struct time
{
    static std::uint32_t get_ms()
    {
        return HAL_GetTick();
    }
};
}; // namespace hal