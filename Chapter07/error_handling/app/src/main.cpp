#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>


int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    uart.write("Hi from main function!\r\n");

    retarget::set_stdio_uart(&uart);

    printf("Printing using printf\r\n");
    
    printf("PC: 0x%08lX\r\n", hal::get_pc());

    while(true)
    {
    }
}
