#include <cstdint>
#include <cstdio>

#include <functional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include "etl/vector.h"
#include <queue>

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    std::priority_queue<int, etl::vector<int, 6>> pq{};
    pq.push(12);
    pq.push(6);
    pq.push(16);
    pq.push(8);
    pq.push(1);
    pq.push(10);

    printf("priority queue elements:\r\n");
    while(!pq.empty()) {
        printf("top element: %d, size: %d\r\n", pq.top(), pq.size());
        pq.pop();
    }

    while(true)
    {
    }
}
