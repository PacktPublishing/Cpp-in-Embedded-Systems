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

struct timer2_traits {
    constexpr static std::uintptr_t base_address = 0x40000000;
    constexpr static IRQn_Type irqn = TIM2_IRQn;

    constexpr static std::uint32_t arr_bit_mask = 0xFFFFFFFF;
};

struct timer3_traits {
    constexpr static std::uintptr_t base_address = 0x40000400;
    constexpr static IRQn_Type irqn = TIM3_IRQn;

    constexpr static std::uint32_t arr_bit_mask = 0xFFFF;
};

template<auto Bits, typename Reg, uint32_t Mask, uint32_t Pos = 0>
struct reg_bits {
    using reg = Reg;
    using T = reg::RegType;

    static_assert(std::is_same_v<T, decltype(Bits)>);

    static constexpr T c_position = Pos;
    static constexpr T c_mask = (Mask << c_position);

    static_assert(Bits <= Mask);

    enum class value : T {
        val = Bits
    };
};

template<typename Reg, uint32_t Pos>
struct reg_bits_enable_disable {
    using reg = Reg;
    using T = reg::RegType;

    static constexpr T c_position = Pos;
    static constexpr T c_mask = (0x1UL << c_position);

    enum class value : T {
        disable = 0,
        enable = 1
    };
};

template <typename TimerTraits>
struct timer {
    constexpr static std::uintptr_t base_address = TimerTraits::base_address;

    using cr1 = reg<base_address + 0x00>;
    using dier = reg<base_address + 0x0C>;
    using sr = reg<base_address + 0x10>;
    using psc = reg<base_address + 0x28>;
    using arr = reg<base_address + 0x2C>;

    template<auto Bits>
    using psc_bits = reg_bits<Bits, psc, static_cast<uint32_t>(0xFFFF)>;

    template<auto Bits>
    using arr_bits = reg_bits<Bits, arr, TimerTraits::arr_bit_mask>;

    using uie = reg_bits_enable_disable<dier, 0UL>;
    using cen = reg_bits_enable_disable<cr1, 0UL>;
    using uif = reg_bits_enable_disable<sr, 0UL>;

    template<std::uint32_t Period>
    static void start() {
        constexpr std::uint32_t prescaler = 10000000 / 1000 - 1;
        constexpr std::uint32_t auto_reload = Period - 1;

        psc::template set<psc_bits<prescaler>>(psc_bits<prescaler>::value::val);
        arr::template set<arr_bits<auto_reload>>(arr_bits<auto_reload>::value::val);

        dier::template set<uie>(uie::value::enable);

        NVIC_SetPriority(TimerTraits::irqn, 1);
        NVIC_EnableIRQ(TimerTraits::irqn);

        cr1::template set<cen>(cen::value::enable);
    }
};

using timer2 = timer<timer2_traits>;
using timer3 = timer<timer3_traits>;

extern "C" void TIM2_IRQHandler(void)
{
    if (timer2::sr::read() & TIM_SR_UIF) 
    {
        timer2::sr::set<timer2::uif>(timer2::uif::value::disable);
        printf("TIM2 IRQ..\r\n");
    }
}

extern "C" void TIM3_IRQHandler(void)
{
    if (timer3::sr::read() & TIM_SR_UIF) 
    {
        timer3::sr::set<timer3::uif>(timer3::uif::value::disable);
        printf("TIM3 IRQ..\r\n");
    }
}

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);
    
    timer2::start<1000>();
    timer3::start<500>();

    while(true)
    {
    }
}
