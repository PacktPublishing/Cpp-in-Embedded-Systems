#include <gpio.hpp>
#include <gpio_interrupt_manager.hpp>

namespace hal {

gpio::gpio(const std::function<void()> & on_press) {
    on_press_ = on_press;
    gpio_interrupt_manager::register_interrupt_handler(this);
}

void gpio::execute_interrupt_handler () const {
    if(is_interrupt_generated())
    {
        clear_interrupt_flag();
        if(on_press_) {
            on_press_();
        }
    }
}

};