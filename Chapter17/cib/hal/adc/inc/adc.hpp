#pragma once

#include <expected>

#include <units.hpp>

namespace hal
{
    class adc {
    public:
        enum class error {
            timeout
        };

        virtual void init() = 0;
        virtual std::expected<units::voltage, adc::error> get_reading() = 0;
    };
};