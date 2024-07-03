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
  std::size_t write_idx = 0; // Index of the next element to write (push)

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
        printf("Unhandled exception!\r\n");
        while(true){}
    });

    std::array<int, 4> arr;

    //arr.at(5) = 6;

    //throw 1;
    try {
      //throw 1;
      arr.at(5) = 6;
      
    }
    catch (int i) {
      printf("i = %d\r\n", i);
    }
    catch(std::out_of_range) {
      printf("out of range\r\n");
    }
    catch (...) {
      printf("Catch\r\n");
    }
    

    while(true)
    {
    }
}
 