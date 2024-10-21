#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

struct read_access{};
struct write_access{};
struct read_write_access : read_access, write_access {};

template<typename BitField, typename Reg, typename T>
concept BitFieldConcept = 
    std::is_same_v<Reg, typename BitField::reg> &&
    std::is_enum_v<typename BitField::value> &&
    std::is_same_v<std::underlying_type_t<typename BitField::value>, T>;


template<std::uintptr_t Address, typename Access = read_write_access, typename T = std::uint32_t>
struct reg {

    using RegType = T;
    using ThisReg = reg<Address, Access, T>; // Type alias for the current instantiation

    template<typename BitField>
    requires BitFieldConcept<BitField, ThisReg, T> // Apply the concept
    static void set(BitField::value bits_val) {
        auto reg_value = read();
        reg_value &= ~BitField::c_mask; 
        reg_value |= (static_cast<T>(bits_val) << BitField::c_position) & BitField::c_mask;
        write(reg_value);
    }

    template <typename Access_ = Access>
    static std::enable_if_t<std::is_base_of_v<read_access, Access_>, T> read() {
        return *reinterpret_cast<volatile T*>(Address);
    }
private:
    template <typename Access_ = Access>
    static std::enable_if_t<std::is_base_of_v<write_access, Access_>, void> write(T val) {
        *reinterpret_cast<volatile T*>(Address) = val;
    }
};

using rcc = reg<0x40021000>;

struct hsion {
    using reg = rcc;
    using T = reg::RegType;

    static constexpr T c_position = 0U;
    static constexpr T c_mask = (1U << c_position);

    enum class value : T {
        disable = 0U,
        enable  = 1U,
    };
};

template<auto Bits>
struct hsi_trim {
    using reg = rcc;
    using T = reg::RegType;

    static_assert(std::is_same_v<T, decltype(Bits)>);

    static constexpr T c_position = 3;
    static constexpr T c_mask = (0x1F << c_position);

    static_assert(Bits <= 0x1F);

    enum class value : T {
        val = Bits
    };
};

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

    rcc::set<hsion>(hsion::value::disable);
    print_reg(&(RCC->CR));

    rcc::set<hsi_trim<0xFLU>>(hsi_trim<0xFLU>::value::val);
    print_reg(&(RCC->CR));

    while(true)
    {
    }
}
