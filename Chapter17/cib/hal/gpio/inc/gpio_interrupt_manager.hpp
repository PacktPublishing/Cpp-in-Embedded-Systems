#pragma once

#include <array>
#include <cstdint>

#include <gpio.hpp>

namespace hal {

    struct gpio_interrupt_manager {
        static void register_interrupt_handler(gpio * pin);

        static void execute_interrupt_handlers();

        static constexpr std::size_t c_gpio_handlers_num = 16;
        static inline std::array<gpio*, c_gpio_handlers_num> gpio_handlers{};
        static inline std::size_t w_idx = 0;
    };
};