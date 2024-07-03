#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <exception>
#include <stdexcept>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

template <class T, std::size_t N> struct ring_buffer {
  std::array<T, N> arr;
  std::size_t write_idx = 0; 

  void push(T t) {
    arr.at(write_idx++) = t;
  }
};


int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    printf("Exceptions example\r\n");

    std::set_terminate([]() {
        printf("My terminate handler!\r\n");
        while(true){}
    });

    std::array<int, 4> arr;

    // uncomment the following line to trigger terminate handler
    // arr.at(5) = 6;

    try {
      arr.at(5) = 6;
    }
    catch(std::out_of_range &e) {
      printf("Array out of range!\r\n");
    }
    catch (...) {
      printf("Unexpected expection...\r\n");
    }

    ring_buffer<int, 4> rb;
    try {
      for(int i = 0; i < 6; i++) {
        rb.push(i);
      }
    }
    catch(std::out_of_range &e) {
      printf("Ring buffer out of range!\r\n");
    } 

    while(true)
    {
    }
}
 