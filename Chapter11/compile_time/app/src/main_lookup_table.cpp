#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <units.hpp>
#include <uart_stm32.hpp>
#include <adc_stm32.hpp>

#include <retarget.hpp>
#include <signal.hpp>

#include <algorithm>
#include <array>

namespace {
    struct voltage_divider {
        units::resistance r2; 
        units::voltage vcc;

        units::resistance get_r1(units::voltage vadc) {
            return r2 * (vcc/vadc - 1);
        }
    };
};

int main()
{
    using namespace units;

    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    hal::adc_stm32 adc(3.3_V);
    adc.init();

    constexpr float A = 1.18090254918130e-3;
    constexpr float B = 2.16884014794388e-4;
    constexpr float C = 1.90058756197216e-6;
    constexpr float D = 1.83161892641824e-8;

    constexpr int c_lut_points = 50;

    constexpr signal<float, c_lut_points> resistance(1e3, 10e3);

    constexpr auto temperature_k = 1 / (A +
                                        B * signal(resistance, [](float x)
                                                 { return std::log(x); }) +
                                        C * signal(resistance, [](float x)
                                                 { return std::pow(std::log(x), 2); }) +
                                        D * signal(resistance, [](float x)
                                                 { return std::pow(std::log(x), 3); }));

    constexpr auto temperature_celsius = temperature_k - 273.15;

    voltage_divider divider{10e3_Ohm, 3.3_V};
    
    while(true)
    {
        auto adc_val = adc.get_reading();
        if(adc_val) {
            auto adc_val_voltage = *adc_val;
            auto thermistor_r = divider.get_r1(adc_val_voltage);

            auto it = std::lower_bound(resistance.begin(), resistance.end(), thermistor_r.get());
            
            if(it != resistance.end()) {

              std::size_t pos = std::distance(resistance.begin(), it);
              float temperature = temperature_celsius.at(pos);

              printf("%d mV, %d Ohm, %d.%d C\r\n", static_cast<int>(adc_val_voltage.get_mili()),
                                        static_cast<int>(thermistor_r.get()),
                                        static_cast<int>(temperature),
                                        static_cast<int>(10*(temperature-std::floor(temperature)))
                                        ); 
            }
        }
        hal::time::delay_ms(200);
    }
}

// adc FeedVoltageSampleToChannel 0 3000 3 -> 1001 Ohm
// adc FeedVoltageSampleToChannel 0 2800 3 -> 1787 Ohm
// adc FeedVoltageSampleToChannel 0 2400 3 -> 3754 Ohm
// adc FeedVoltageSampleToChannel 0 2200 3 -> 5003 Ohm
// adc FeedVoltageSampleToChannel 0 2000 3 -> 6502 Ohm
// adc FeedVoltageSampleToChannel 0 1700 3 -> 9412 Ohm