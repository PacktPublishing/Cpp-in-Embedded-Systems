#include <gpio_interrupt_manager.hpp>

namespace hal {

void gpio_interrupt_manager::register_interrupt_handler(gpio * pin) {
    gpio_handlers.at(w_idx++) = pin;
}

void gpio_interrupt_manager::execute_interrupt_handlers() {
    for(std::size_t i = 0; i < w_idx; i++) {
        gpio_handlers[i]->execute_interrupt_handler();
    }
}
extern "C" void EXTI4_15_IRQHandler(void) {
    gpio_interrupt_manager::execute_interrupt_handlers();
}
};