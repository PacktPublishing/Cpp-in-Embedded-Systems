#pragma once

#include <cstdint>

namespace hal
{
class uart
{
  public:
    virtual void init() = 0;

    virtual void putchar(char c) = 0;

    void puts(const char *str)
    {
        std::size_t ind = 0;

        if(str != nullptr)
        {
            while(str[ind] != '\0')
            {
                putchar(str[ind]);
                ind++;
            }
        }
    }
};
}; // namespace hal
