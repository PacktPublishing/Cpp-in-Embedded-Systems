#include <cstdint>
#include <cstdio>

#include <functional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include <cassert>

#include "sml.hpp"

namespace sml = boost::sml;

struct ble_button_pressed{};
struct connection_request{};
struct timer_expired{};

constexpr auto start_advertising = [](){
    printf("Action: start_advertising()\n");
};

constexpr auto stop_advertising = [](){
    printf("Action: stop_advertising()\n");
};

constexpr auto disconnect = [](){
    printf("Action: disconnect()\n");
};

struct ble_fsm {
  auto operator()() const {
    using namespace sml;
    /**
     * Initial state: *initial_state
     * Transition DSL: src_state + event [ guard ] / action = dst_state
     */
    return make_transition_table(
      *"idle"_s + event<ble_button_pressed>         / start_advertising = "advertising"_s,
       "advertising"_s  + event<connection_request>                     = "connected"_s,
       "advertising"_s  + event<timer_expired>      / stop_advertising  = "idle"_s,
       "connected"_s + event<ble_button_pressed>    / disconnect        = "idle"_s
    );
  }
};

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    using namespace sml;

    sm<ble_fsm> my_ble_fsm{}; 

    const auto print_current_state = [&]() {
        printf("Current State: ");
        if(my_ble_fsm.is("idle"_s)) {
            printf("idle\n");
        } 
        if(my_ble_fsm.is("advertising"_s)) {
            printf("advertising\n");
        } 
        if(my_ble_fsm.is("connected"_s)) {
            printf("connected\n");
        } 
    };

    print_current_state();

    my_ble_fsm.process_event(ble_button_pressed{});
    print_current_state();

    my_ble_fsm.process_event(connection_request{});
    print_current_state();

    my_ble_fsm.process_event(ble_button_pressed{});
    print_current_state();

    while(true)
    {
    }
}
