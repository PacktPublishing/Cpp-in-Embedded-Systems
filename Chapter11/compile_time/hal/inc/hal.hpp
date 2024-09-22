#pragma once

#include <cstdint>

#include <stm32f0xx_hal.h>

namespace hal
{
inline void init()
{
    HAL_Init();
}

inline std::uint32_t get_pc() 
{
    std::uint32_t pc;
    __asm volatile ("mov %0, pc" : "=r" (pc) );
    return pc;
}

struct time
{
    static std::uint32_t get_ms()
    {
        return HAL_GetTick();
    }

    static void delay_ms(uint32_t delay) 
    {
        HAL_Delay(delay);
    }
};
}; // namespace hal