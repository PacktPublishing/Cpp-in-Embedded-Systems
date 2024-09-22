#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include <algorithm>
#include <array>
#include <string_view>
#include <stdexcept>
#include <charconv>

struct mac_address {
    static constexpr std::size_t c_bytes_num = 6;
    static constexpr std::size_t c_mac_addr_str_size = 17;

    std::array<uint8_t, c_bytes_num> bytes{};
    bool is_valid = false;

    constexpr mac_address(std::string_view str) {
        if (str.size() != c_mac_addr_str_size) {
            return;
        }

        for (size_t i = 0; i < c_bytes_num; ++i) {
            const std::string_view byte_str = str.substr(i * 3, 2);

            uint8_t value = 0;
            auto result = std::from_chars(byte_str.data(), byte_str.data() + byte_str.size(), value, 16);
            if (result.ec != std::errc()) {
                return;
            }
            bytes[i] = value;
        }
        is_valid = true;
    }
};

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);
    
    constexpr mac_address addr("00:11:22:33:44:55");
    static_assert(addr.is_valid);

    const std::array<uint8_t, 6> addr_arr{0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

    const auto & mac_ref = addr.bytes;
    //const auto & mac_ref = addr_arr;

    printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", mac_ref[0], mac_ref[1], mac_ref[2],
                                               mac_ref[3], mac_ref[4], mac_ref[5]);
    
    while(true)
    {
    }
}
