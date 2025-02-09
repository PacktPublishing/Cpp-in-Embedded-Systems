#pragma once

#include <cstdint>
#include <span>

namespace hal
{
class uart
{
  public:
    virtual void init(std::uint32_t baudrate) = 0;
    virtual void write(std::span<const char> data) = 0;
};
}; // namespace hal
