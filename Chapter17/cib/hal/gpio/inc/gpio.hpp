#pragma once

#include <cstdint>
#include <functional>

namespace hal
{
class gpio
{
public:
    gpio(const std::function<void()> & on_press = nullptr);
    void execute_interrupt_handler() const;

    [[nodiscard]] virtual bool is_interrupt_generated() const = 0;
    virtual void clear_interrupt_flag() const = 0;
private:
    std::function<void()> on_press_;
};
}; // namespace hal
