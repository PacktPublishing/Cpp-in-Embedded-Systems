#include <cstdint>

#include <hal.hpp>
#include <uart_stm32.hpp>
#include <stm32f072xb.h>

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    uart.puts("Hello world !\r\n");

    std::uint32_t time_prev = hal::time::get_ms();
    constexpr std::uint32_t c_1000_ms = 1000;
    while(true)
    {
        if(auto time_now = hal::time::get_ms();
           time_now - time_prev > c_1000_ms)
        {
            uart.puts("While loop 1000 ms ping ...\r\n");
            time_prev = time_now;
        }
    }
}
