#include <algorithm>
#include <cstdint>
#include <cstdio>

#include <functional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include <cassert>

#include <cib/cib.hpp>

struct runtime_init : public flow::service<> {};

struct main_loop : public callback::service<> {};

struct send_temperature : public callback::service<float> {};

struct i2c {

    constexpr static auto init = flow::action<"i2c_init">(
        [](){
            printf("I2C init ...\r\n");
        });

    constexpr static auto config = cib::config(
        cib::extend<runtime_init>(*init)
    );

};

struct temperature_sensor_component {

    constexpr static auto init = flow::action<"temp_sensor_init">(
        []() {
            printf("Initializing temperature sensor ... \r\n");
        });

    constexpr static auto read_temperature = []() {
        float temperature = 23.4f;

        cib::service<send_temperature>(temperature);
    };

    constexpr static auto config = cib::config(
        cib::extend<main_loop>(read_temperature),

        cib::extend<runtime_init>(i2c::init >> *init)
    );

};

struct display_temperature_component {

    constexpr static auto display_temperature = [](float temperature) {
        printf("Temperature is %.2f C\r\n", temperature);
    };

    constexpr static auto config = cib::config(
        cib::extend<send_temperature>(display_temperature)
    );

};

struct data_sender_component {

    constexpr static auto send_temp = [](float temp) {
        printf("Sending temperature %.2f C\r\n", temp);
    };

    constexpr static auto config = cib::config(
        cib::extend<send_temperature>(send_temp)
    );

};

struct my_project {
    
    constexpr static auto config = cib::config(
        cib::exports<runtime_init,
                    main_loop,
                    send_temperature>,
                        
        cib::components<i2c,
                        temperature_sensor_component,
                        display_temperature_component,
                        data_sender_component>
    );
};

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    cib::nexus<my_project> nexus{};
    nexus.init();

    nexus.service<runtime_init>();

    for(int i = 0; i < 3; i++)
    {
        nexus.service<main_loop>();
    }

    while(true)
    {
    }
}
