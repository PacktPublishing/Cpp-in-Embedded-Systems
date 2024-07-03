#include <cstdint>
#include <cstdio>
#include <cstdlib>


#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    printf("std::expected example\r\n");

    while(true)
    {
    }
}
 