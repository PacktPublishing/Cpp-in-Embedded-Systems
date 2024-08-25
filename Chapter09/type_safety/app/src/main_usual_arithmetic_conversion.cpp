#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>
#include <cstring>

#include <type_traits>

struct my_struct {
    char c;
    int a;
};

void print_my_struct (const my_struct & str) {
    printf("c = %d, a = %d\r\n", str.c, str.a);
}

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

#if 0
    char c;
    char arr[128];
    arr[0] = 'A';
    my_struct str{'X', 42};

    constexpr size_t offset = 3;

    memcpy(&arr[offset], &str, sizeof(my_struct));

    auto *pstr = reinterpret_cast<my_struct *>(&arr[offset]);

    printf("arr[0] = %d\r\n", arr[0]);

    print_my_struct(str);
    print_my_struct(*pstr);
#endif
    struct bitfield{ 

        long long a:31;  

    }; 

    bitfield b {4}; 

    int c = 1; 

    auto res1 = b.a + c;    

    static_assert(sizeof(int) == 4); 

    static_assert(sizeof(long long) == 8); 

    static_assert(std::is_same_v<int, decltype(res1)>);  

  

    long e = 5;  

    auto res2 = e - b.a;  

    static_assert(std::is_same_v<long, decltype(res2)>);  

    printf("res2 = %ld\r\n", res2);
    
    while(true)
    {
    }
}
