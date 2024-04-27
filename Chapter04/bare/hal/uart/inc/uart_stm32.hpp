#pragma once

#include <cstdint>

#include <uart.hpp>

#include <stm32f0xx_hal.h>

namespace hal
{
class uart_stm32 : public uart
{
  public:
    uart_stm32(USART_TypeDef *inst, std::uint32_t baud = 115200);

    void init() override;

    void putchar(char c) override;

  private:
    UART_HandleTypeDef huart;
    USART_TypeDef *instance;
    std::uint32_t baudrate;
};
}; // namespace hal
