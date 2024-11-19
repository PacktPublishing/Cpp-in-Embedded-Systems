#include <cstdint>
#include <cstdio>

#include <functional>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include "etl/delegate.h"
#include "etl/vector.h"
#include <queue>

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

template<typename Task, std::size_t Size>
struct sequencer {

    sequencer() = delete;

    static void add(Task task) {
        if(pq.size() < Size) {
            __disable_irq();
            pq.push(task);
            __enable_irq();
        }
    }

    static void run() {
        if(!pq.empty()) {
            __disable_irq();
            auto task = pq.top();
            pq.pop();
            __enable_irq();
            task.execute();
        }
    }

private:
    static inline std::priority_queue<Task, etl::vector<Task, Size>> pq{};

};

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

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

    task_etl task_member_fun(callable_etl::create<test, &test::print>(test_1), 20);

    task_etl task_static_fun(callable_etl::create<test::print_static>(), 30);

    task_etl task_lambda([](){
        printf("This is non capturing lambda!\r\n");
    }, 10);

    using seq = sequencer<task_etl, 16>;
    seq::add(task_member_fun);
    seq::add(task_static_fun);
    seq::add(task_lambda);

    while(true)
    {
        seq::run();
    }
}
