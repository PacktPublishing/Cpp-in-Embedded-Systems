#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <expected>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

struct ble_light_bulb {
    enum class error {
        disconnected,
        timeout
    };
    struct config {
        int r;
        int g;
        int b;
    };

    bool ret_val;
    std::expected<config, error> get_config() {
        ret_val = !ret_val;
        if(ret_val) {
            return config {10, 20, 30};
        }
        else {
            return std::unexpected(error::timeout);
        }
    }
};

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    printf("std::expected example\r\n");

        ble_light_bulb bulb;

    const auto get_config_from_main = [&bulb]() {
        auto result = bulb.get_config();
        if(result.has_value()) {
            auto conf = result.value();
            printf("Config r %d, g %d, b %d\r\n", conf.r, conf.g, conf.b);
        } else {
            auto err = result.error();
            if(err == ble_light_bulb::error::disconnected) {
                printf("The bulb is disconnected! \r\n");
            }
            else if(err == ble_light_bulb::error::timeout) {
                printf("Timeout!\r\n");
            }
        }
    };

    get_config_from_main();
    get_config_from_main();

    while(true)
    {
    }
}
 