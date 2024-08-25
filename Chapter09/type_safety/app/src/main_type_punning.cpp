#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>
#include <cstring>

#include <type_traits>
#include <bit>

namespace {

struct my_struct {
    int a;
    char c;
};

void print_my_struct (const my_struct & str) {
    printf("a = %d, c = %c\r\n", str.a, str.c);
}

void process_data(const char * data) {
    const auto *pstr = reinterpret_cast<const my_struct *>(data);

    printf("%s\r\n", __func__);
    print_my_struct(pstr[0]);
    print_my_struct(pstr[1]);
}

void process_data_memcpy(const char * data) {
    my_struct my_structs[2];
    std::memcpy(my_structs, data, sizeof(my_structs));

    printf("%s\r\n", __func__);
    print_my_struct(my_structs[0]);
    print_my_struct(my_structs[1]);
}
};

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    int i = 42;
    auto * i_ptr = reinterpret_cast<char*>(&i);

    if(i_ptr[0]==42) {
        printf("Little endian!\r\n");
    } 
    else {
        printf("Big endian!\r\n");
    }

    my_struct my_structs_arr[] = {{4, 'a'}, {5, 'b'}};
    char arr[128];
    std::memcpy(&arr, my_structs_arr, sizeof(my_structs_arr));

    process_data(arr);
    process_data_memcpy(arr);

    while(true)
    {
    }
}
