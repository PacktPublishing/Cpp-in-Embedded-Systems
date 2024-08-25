#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>
#include <cstring>

#include <type_traits>

namespace {

constexpr uint8_t c_PUBLIC_ADDR = 0x01;
constexpr uint16_t c_SUPERV_TIMEOUT = 0x0C80;

class time {
private:
	uint16_t time_in_ms_;
public:
	explicit time(uint16_t time_in_ms) : time_in_ms_(time_in_ms){}
	uint16_t & get_ms() {return time_in_ms_;}
};

time operator""_ms(unsigned long long t) {
    return time(t);
}

class conn_l {
private:
	uint16_t val_; 
public:
	explicit conn_l(time t) : val_(t.get_ms()/0.625f){}
	uint16_t & get() {return val_;}
};

class conn_p {
private:
	uint16_t val_; 
public:
	explicit conn_p(time t) : val_(t.get_ms()/1.25f){}
	uint16_t & get() {return val_;}
};


using tBleStatus = bool;

tBleStatus aci_gap_create_connection_wrapper( 
                            conn_l LE_Scan_Interval, 
                            conn_l LE_Scan_Window, 
                            uint8_t Peer_Address_Type, 
                            uint8_t Peer_Address[6], 
                            conn_p Conn_Interval_Min, 
                            conn_p Conn_Interval_Max, 
                            time Conn_Latency, 
                            time Supervision_Timeout, 
                            conn_l Minimum_CE_Length, 
                            conn_l Maximum_CE_Length) {
                                static_cast<void>(LE_Scan_Interval);
                                static_cast<void>(LE_Scan_Window);
                                static_cast<void>(Peer_Address_Type);
                                static_cast<void>(Peer_Address);
                                static_cast<void>(Conn_Interval_Min);
                                static_cast<void>(Conn_Interval_Max);
                                static_cast<void>(Conn_Latency);
                                static_cast<void>(Supervision_Timeout);
                                static_cast<void>(Minimum_CE_Length);
                                static_cast<void>(Maximum_CE_Length);

                                return true;
                            }
};

int main()
{
    hal::init();

    hal::uart_stm32 uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    const tBleStatus stat = aci_gap_create_connection_wrapper(
            conn_l(80_ms),
            conn_l(120_ms),
            c_PUBLIC_ADDR,
            nullptr,
            conn_p(50_ms),
            conn_p(60_ms),
            0_ms,
            4000_ms,
            conn_l(10_ms),
            conn_l(15_ms)
    );

    static_cast<void>(stat);
    
    while(true)
    {
    }
}
