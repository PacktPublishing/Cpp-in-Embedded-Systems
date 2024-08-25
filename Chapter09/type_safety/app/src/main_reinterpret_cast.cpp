#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>
#include <cstring>

#include <type_traits>

int fun() {
    printf("fun\r\n");
    return 42;
}

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);
    float f = 3.14f;

    // initialize pointer to an int with float address
    auto a = reinterpret_cast<int*>(&f);
    printf("a = %d\r\n", *a);

    // the above is same as:
    a = static_cast<int*>(static_cast<void*>(&f));
    printf("a = %d\r\n", *a);

    // casting back to float pointer
    auto fptr = reinterpret_cast<float*>(a);
    printf("f = %.2f\r\n", *fptr);

    // converting a pointer to integer
    auto int_val = reinterpret_cast<std::uintptr_t>(fptr);
    printf("Address of float f is 0x%8X\r\n", int_val);

    auto fun_void_ptr = reinterpret_cast<void(*)()>(fun);
    // undefined behavior
    fun_void_ptr();

    auto fun_int_ptr = reinterpret_cast<int(*)()>(fun);
    // safe call
    printf("fun_int_ptr returns %d\r\n", fun_int_ptr());
    
    while(true)
    {
    }
}
