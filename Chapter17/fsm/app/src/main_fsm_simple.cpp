#include <cstdint>
#include <cstdio>

#include <functional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

enum class ble_state {
    idle,
    advertising,
    connected
};

enum class ble_event {
    ble_button_pressed,
    connection_request,
    timer_expired
};

class ble_fsm {
public:
    void handle_event(ble_event event) {
        switch (current_state_) {
            case ble_state::idle:
                if (event == ble_event::ble_button_pressed) {
                    start_advertising();
                    current_state_ = ble_state::advertising;
                }
                break;

            case ble_state::advertising:
                if (event == ble_event::connection_request) {
                    current_state_ = ble_state::connected;
                } else if (event == ble_event::timer_expired) {
                    stop_advertising();
                    current_state_ = ble_state::idle;
                }
                break;

            case ble_state::connected:
                if (event == ble_event::ble_button_pressed) {
                    disconnect();
                    current_state_ = ble_state::idle;
                }
                break;
            default:
                break;
        }
    }

    ble_state get_state() const {
        return current_state_;
    }

private:
    ble_state current_state_ = ble_state::idle;

    void start_advertising() {
        printf("Action: start_advertising()\n");
    }

    void stop_advertising() {
        printf("Action: stop_advertising()\n");
    }

    void disconnect() {
        printf("Action: disconnect()\n");
    }
};

static const char* state_to_string(ble_state state) {
    switch (state) {
        case ble_state::idle:        return "idle";
        case ble_state::advertising: return "advertising";
        case ble_state::connected:   return "connected";
        default:                     return "unknown";
    }
}

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    ble_fsm my_ble_fsm;

    const auto print_current_state = [&]() {
        printf("Current State: %s\n", state_to_string(my_ble_fsm.get_state()));
    };

    print_current_state();

    my_ble_fsm.handle_event(ble_event::ble_button_pressed);
    print_current_state();

    my_ble_fsm.handle_event(ble_event::connection_request);
    print_current_state();

    my_ble_fsm.handle_event(ble_event::ble_button_pressed);
    print_current_state();


    while(true)
    {
    }
}
