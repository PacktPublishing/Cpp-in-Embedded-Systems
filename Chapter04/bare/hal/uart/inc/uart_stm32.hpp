#pragma once

#include <cstdint>

#include <uart.hpp>

#include <stm32f0xx_hal.h>
#include <stm32f072xb.h>

namespace hal
{
class uart_stm32 : public uart
{
  public:
    uart_stm32(USART_TypeDef *inst, std::uint32_t baud = baudrate_default);

    void init() override;

    void putchar(char tx_char) override;

  private:
    UART_HandleTypeDef huart_;
    USART_TypeDef *instance_;
    std::uint32_t baudrate_;
    static constexpr std::uint32_t baudrate_default = 115200;
};
}; // namespace hal
