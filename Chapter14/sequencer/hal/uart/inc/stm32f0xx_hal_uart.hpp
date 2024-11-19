#pragma once

#include <stm32f0xx_hal.h>
#include <stm32f072xb.h>

namespace hal::stm32{
    struct uart {
        uart() = delete;

        static inline HAL_StatusTypeDef init(UART_HandleTypeDef *huart) {
            return HAL_UART_Init(huart);
        }

        static inline HAL_StatusTypeDef transmit(UART_HandleTypeDef *huart, 
                                                uint8_t *pData, 
                                                uint16_t Size, 
                                                uint32_t Timeout) {
            return HAL_UART_Transmit(huart, pData, Size, Timeout);
        }
    };
};