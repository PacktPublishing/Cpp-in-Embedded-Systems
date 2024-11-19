#include <cstdint>
#include <cstdio>

#include <functional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include "etl/delegate.h"

template<typename CallableHolder>
class task {
public:
    constexpr static std::uint8_t c_prio_default = 250;
    constexpr static std::uint8_t c_prio_max = 255;
    constexpr static std::uint8_t c_prio_min = 0;

    task(CallableHolder the_task, std::uint8_t prio = c_prio_default) : 
                    the_task_(the_task), priority_(prio) {}

    void execute() {
        if(the_task_) {
            the_task_();
        }
    }

    bool operator<(const task &rhs) const
    {
        return priority_ < rhs.priority_;
    }

private:
    CallableHolder the_task_;
    std::uint8_t priority_ = c_prio_default;
};

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    using callable_etl = etl::delegate<void()>;
    using callable_holder = callable_etl;
    //using callable_holder = std::function<void()>;

    auto fun_a = []() {
        printf("High priority task!\r\n");
    };
    task<callable_holder> task_a(fun_a, 255);

    auto fun_b = []() {
        printf("Low priority task!\r\n");
    };
    task<callable_holder> task_b(fun_b, 20);

    if(task_a < task_b) {
        task_b.execute();
    } 
    else {
        task_a.execute();
    }

    using callable_etl = etl::delegate<void()>;
    using task_etl = task<callable_etl>;

    class test {
    public:
        test(int x) : x_(x) {}
        void print() const {
            printf("This is a test, x = %d.\r\n", x_);
        }
        void static print_static() {
            printf("This is a static method in test.\r\n");
        }
    private:
        int x_ = 0;
    };

    test test_1(42);

    task_etl task_member_fun(callable_etl::create<test, &test::print>(test_1));
    task_member_fun.execute();

    task_etl task_static_fun(callable_etl::create<test::print_static>());
    task_static_fun.execute();

    task_etl task_lambda([](){
        printf("This is non capturing lambda!\r\n");
    });
    task_lambda.execute();

    while(true)
    {
    }
}
