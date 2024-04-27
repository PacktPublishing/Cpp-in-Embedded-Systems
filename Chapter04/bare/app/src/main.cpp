#include <cstdint>

#include <stm32f0xx_hal.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

extern "C" int main(void)
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    uart.puts("Hello world !\r\n");

    std::uint32_t time_prev = hal::time::get_ms();
    while(1)
    {
        if(volatile auto time_now = hal::time::get_ms();
           time_now - time_prev > 1000)
        {
            uart.puts("While loop 1000 ms ping ...\r\n");
            time_prev = time_now;
        }
    }
}

