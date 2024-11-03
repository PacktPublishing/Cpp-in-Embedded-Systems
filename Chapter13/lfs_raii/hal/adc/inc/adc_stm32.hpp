#pragma once

#include <adc.hpp>
#include <units.hpp>

#include <stm32f0xx_hal.h>
#include <stm32f072xb.h>

namespace hal 
{

    class adc_stm32 : public adc{
    public:
        adc_stm32(units::voltage ref_voltage) : ref_voltage_(ref_voltage) {}

        void init() override;
        std::expected<units::voltage, adc::error> get_reading() override;
    private:
        ADC_HandleTypeDef adc_handle_;
        units::voltage ref_voltage_;
    };
};