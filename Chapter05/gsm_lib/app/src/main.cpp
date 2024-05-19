#include <cstdint>


#include <span>

#include <cstdio>
#include <cstdint>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>



class gsm_lib{
    public:
        gsm_lib(hal::uart &u) : uart_(u) {}
        void init() {
            uart_.write("AT\r\n");
        }
    private:
        hal::uart &uart_;

};


extern "C" int main(void)
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    uart.write("Hi from main function!\r\n");

    gsm_lib gsm(uart);
    gsm.init();
    
    while(true)
    {
    }
}
