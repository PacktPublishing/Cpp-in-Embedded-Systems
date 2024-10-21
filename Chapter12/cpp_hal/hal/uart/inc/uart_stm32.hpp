#pragma once

#include <span>

#include <cstdint>

#include <uart.hpp>

#include <stm32f0xx_hal.h>
#include <stm32f072xb.h>

namespace hal
{
class uart_stm32 : public uart
{
  public:
    uart_stm32(USART_TypeDef *inst);

    void init(std::uint32_t baudrate = c_baudrate_default) override;

    void write_array(const char * ptr, std::size_t len) override;

  private:
    UART_HandleTypeDef huart_;
    USART_TypeDef *instance_;
    std::uint32_t baudrate_;
    static constexpr std::uint32_t c_baudrate_default = 115200;
};
}; // namespace hal
