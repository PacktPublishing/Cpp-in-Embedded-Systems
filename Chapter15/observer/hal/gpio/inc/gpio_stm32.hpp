#include <gpio.hpp>

#include <stm32f072xb.h>

namespace hal {

enum class pin : uint16_t {
        p_invalid = 0,
        p0 = 0x0001U,
        p1 = 0x0002U,
        p2 = 0x0004U,
        p3 = 0x0008U,
        p4 = 0x0010U,
        p5 = 0x0020U,
        p6 = 0x0040U,
        p7 = 0x0080U,
        p8 = 0x0100U,
        p9 = 0x0200U,
        p10 = 0x0400U,
        p11 = 0x0800U,
        p12 = 0x1000U,
        p13 = 0x2000U,
        p14 = 0x4000U,
        p15 = 0x8000U,
        all = 0xFFFFU
};

struct port_a {
    static constexpr uint8_t c_pin_num = 16;
    static inline GPIO_TypeDef * port = reinterpret_cast<GPIO_TypeDef*>(GPIOA);
    static void init_clock () {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
};

template<typename Port>
class gpio_stm32 : public gpio {
public:
    gpio_stm32(pin the_pin, const std::function<void()> & on_press = nullptr) 
    : gpio(on_press), the_pin_(the_pin) {
        Port::init_clock();

        GPIO_InitTypeDef GPIO_InitStruct { static_cast<uint16_t>(the_pin),
                                            GPIO_MODE_IT_RISING,
                                            GPIO_NOPULL,
                                            GPIO_SPEED_FREQ_LOW,
                                            0 };
        HAL_GPIO_Init(Port::port, &GPIO_InitStruct);

        if(on_press) {
            enable_interrupt();
        }
    }

    [[nodiscard]] bool is_interrupt_generated() const override {
        return __HAL_GPIO_EXTI_GET_IT(static_cast<uint16_t>(the_pin_));
    }
    void clear_interrupt_flag() const override {
        __HAL_GPIO_EXTI_CLEAR_IT(static_cast<uint16_t>(the_pin_));
    }
private:
    pin the_pin_ = pin::p_invalid;

    void enable_interrupt() {
        // TODO: check EXTI line macro according to pin used
        HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
    }
};
};