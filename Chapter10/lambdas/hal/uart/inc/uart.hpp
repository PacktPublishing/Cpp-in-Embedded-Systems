#pragma once

#include <cstdint>

#include <span>

namespace hal
{
class uart
{
  public:
    virtual void init(std::uint32_t baudrate) = 0;
    virtual void write_array(const char * ptr, std::size_t len) = 0;

    virtual void write(std::span<const char> data)
    {
      write_array(data.data(), data.size());
    }
};
}; // namespace hal
