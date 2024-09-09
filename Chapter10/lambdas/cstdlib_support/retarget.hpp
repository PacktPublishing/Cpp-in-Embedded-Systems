#pragma once

#include <uart.hpp>

namespace retarget 
{

void set_stdio_uart(hal::uart *uart);

};