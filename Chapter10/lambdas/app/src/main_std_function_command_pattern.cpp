#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include <algorithm>
#include <array>

#include <gpio_stm32.hpp>

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    const hal::gpio_stm32<hal::port_a> button1(hal::pin::p4, [](){
        printf("Button1 pressed!\r\n");
    });

    const hal::gpio_stm32<hal::port_a> button2(hal::pin::p5, [](){
        printf("Button2 pressed!\r\n");
    });
    
    while(true)
    {
    }
}
