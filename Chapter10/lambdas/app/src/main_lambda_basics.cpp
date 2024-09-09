#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include <algorithm>
#include <array>


int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);
    
    std::array<int, 4> arr{5, 3, 4, 1};

    const auto print_arr = [&arr](const char* message) {
        printf("%s\r\n", message);
        for(auto elem : arr) {
            printf("%d, ", elem);
        }
        printf("\r\n");
    };

    print_arr("Unsorted array:");

    std::sort(arr.begin(), arr.end(), [](int a, int b) {
       return a < b;});
    print_arr("Sorted in ascending order:");


    std::sort(arr.begin(), arr.end(), [](int a, int b) {
       return a > b;});
    print_arr("Sorted in descending order:");
    
    while(true)
    {
    }
}
