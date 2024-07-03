#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

struct sensor {
    struct data {
        int x;
        int y;
    };
    static inline bool ret_val = true;
    static std::optional<data> get_data() {
        ret_val = !ret_val;
        if(ret_val) {
            return data{4, 5};
        } 
        else {
            return std::nullopt;
        }
    }
};

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    printf("std::optional example\r\n");

    auto get_data_from_main = [] () {
      auto result = sensor::get_data();
      if(result) {
          printf("x = %d, y = %d\r\n", (*result).x, (*result).y);
      } 
      else {
          printf("No data!\r\n");
      }
    };

    get_data_from_main();
    get_data_from_main();

    while(true)
    {
    }
}
 