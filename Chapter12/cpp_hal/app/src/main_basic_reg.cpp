#include <cstdint>
#include <cstdio>
#include <type_traits>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

struct read_access{};
struct write_access{};
struct read_write_access : read_access, write_access {};

template<std::uintptr_t Address, typename Access = read_write_access, typename T = std::uint32_t>
struct reg {

    template <typename Access_ = Access>
    static std::enable_if_t<std::is_base_of_v<read_access, Access_>, T> read() {
        return *reinterpret_cast<volatile T*>(Address);
    }

    template <typename Access_ = Access>
    static std::enable_if_t<std::is_base_of_v<write_access, Access_>, void> write(T val) {
        *reinterpret_cast<volatile T*>(Address) = val;
    }
};

using rcc = reg<0x40021000, read_write_access>;


int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);
    
    printf("Hello world\r\n");

    auto const print_reg = [](volatile uint32_t * reg) {
        printf("========================\r\n");
        printf("Reg address = %p\r\n", reinterpret_cast<volatile void*>(reg));
        printf("Reg value   = 0x%08lX\r\n", *reg);
    };

    print_reg(&(RCC->CR));

    // RCC->CR = 0xDEADBEEF;
    rcc::write(0xDEADBEEF);
    auto val = rcc::read();
    printf("val = %ld\r\n", val);

    print_reg(&(RCC->CR));
    
    while(true)
    {
    }
}
