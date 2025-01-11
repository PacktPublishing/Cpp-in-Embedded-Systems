#include <cstdint>
#include <cstdio>

#include <functional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include "etl/vector.h"

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

class state {
public:
    virtual ble_state handle_event(ble_event event) = 0;
    virtual ble_state get_state_enum() = 0;
};

class idle : public state{
public:
    ble_state handle_event(ble_event event) {
        if (event == ble_event::ble_button_pressed) {
            start_advertising();
            return ble_state::advertising;
        }
        return get_state_enum();
    }
    ble_state get_state_enum() {
       return ble_state::idle; 
    }
private:
    void start_advertising() {
        printf("Action: start_advertising()\n");
    }
};

class advertising : public state{
public:
    ble_state handle_event(ble_event event) {
        if (event == ble_event::connection_request) {
            return ble_state::connected;
        }
        if (event == ble_event::timer_expired) {
            stop_advertising();
            return ble_state::idle;
        }
        return get_state_enum();
    }
    ble_state get_state_enum() {
       return ble_state::advertising; 
    }
private:
    void stop_advertising() {
        printf("Action: stop_advertising()\n");
    }
};

class connected : public state{
public:
    ble_state handle_event(ble_event event) {
        if (event == ble_event::ble_button_pressed) {
            disconnect();
            return ble_state::idle;
        }
        return get_state_enum();
    }
    ble_state get_state_enum() {
       return ble_state::connected; 
    }
private:
    void disconnect() {
        printf("Action: disconnect()\n");
    }
};

class ble_fsm {
public:
    void handle_event(ble_event event) {
        if(auto the_state = get_the_state(current_state_)) {
            current_state_ = the_state->handle_event(event);
        }
    }

    ble_state get_state() const {
        return current_state_;
    }

    void add_state(state *the_state) {
        states_.push_back(the_state);
    }

private:
    std::vector<state*> states_;
    ble_state current_state_ = ble_state::idle;

    state* get_the_state(ble_state state_enum) {
        const auto is_state_enum = [&] (state *the_state) {
            return the_state->get_state_enum() == state_enum;
        };
        auto it = std::find_if(states_.begin(), states_.end(), is_state_enum);

        if(it != states_.end()) {
            return *it;
        }
        return nullptr;
    }
};

namespace {
const char* state_to_string(ble_state state) {
    switch (state) {
        case ble_state::idle:        return "idle";
        case ble_state::advertising: return "advertising";
        case ble_state::connected:   return "connected";
        default:                     return "unknown";
    }
}
}


int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    ble_fsm my_ble_fsm;

    idle idle_s;
    advertising advertising_s;
    connected connected_s;

    my_ble_fsm.add_state(&idle_s);
    my_ble_fsm.add_state(&advertising_s);
    my_ble_fsm.add_state(&connected_s);

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
