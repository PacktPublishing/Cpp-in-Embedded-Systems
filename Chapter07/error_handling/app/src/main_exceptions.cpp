#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

void log_pc_and_halt(std::uint32_t pc) {
    printf("Assert at 0x%08lX\r\n", pc);
    while(true) {}
}

#define light_assert(expr)         \
        (static_cast<bool> (expr)  \
        ? void (0)                 \
        : log_pc_and_halt(hal::get_pc())    \
        )

enum class option : std::uint8_t {
    Option1 = 0, 
    Option2, 
    Option3,
    Last
};
    
option uint8_to_option(uint8_t num) {
    light_assert(num < static_cast<uint8_t>(option::Last));
    return static_cast<option>(num);
}

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);
    
    printf("Exceptions example\r\n");

    while(true)
    {
    }
}
