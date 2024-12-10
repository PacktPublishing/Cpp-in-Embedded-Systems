#include <cstdint>
#include <cstdio>

#include <functional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

struct display {
    static void update(float temp) {
        printf("Displaying temperature %.2f \r\n", temp);
    }
};

struct data_sender {
    static void update(float temp) {
        printf("Sending temperature %.2f \r\n", temp);
    }
};

struct logger {
    static void update(float temp) {
        printf("Logging temperature %.2f \r\n", temp);
    }
};

template <typename... Subs>
struct publisher {
    static void notify(float temp) {
        (Subs::update(temp), ...);
    }
};

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    using temp_publisher = publisher<display, data_sender, logger>;
    temp_publisher::notify(23.47);

    while(true)
    {
    }
}
