#include <uart_stm32.hpp>

template <typename HalUart>
hal::uart_stm32<HalUart>::uart_stm32(USART_TypeDef *inst)
    : instance_(inst)
{
}

template <typename HalUart>
void hal::uart_stm32<HalUart>::init(std::uint32_t baudrate)
{
    huart_.Instance = instance_;
    huart_.Init.BaudRate = baudrate;
    huart_.Init.WordLength = UART_WORDLENGTH_8B;
    huart_.Init.StopBits = UART_STOPBITS_1;
    huart_.Init.Parity = UART_PARITY_NONE;
    huart_.Init.Mode = UART_MODE_TX_RX;
    huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart_.Init.OverSampling = UART_OVERSAMPLING_16;
    huart_.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart_.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    // TODO: add GPIO initialization for real hardware
    huart_.MspInitCallback = nullptr;
    HalUart::init(&huart_);
}

template <typename HalUart>
void hal::uart_stm32<HalUart>::write(std::span<const char> data) 
{
    // we must cast away costness due to ST HAL's API
    char * data_ptr = const_cast<char *>(data.data());
    HalUart::transmit(&huart_, reinterpret_cast<uint8_t *>(data_ptr), data.size(),
                      HAL_MAX_DELAY);
}