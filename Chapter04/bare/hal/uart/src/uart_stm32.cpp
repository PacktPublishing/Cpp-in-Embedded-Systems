#include <uart_stm32.hpp>

hal::uart_stm32::uart_stm32(USART_TypeDef *inst, std::uint32_t baud)
    : instance_(inst), baudrate_(baud)
{
}

void hal::uart_stm32::init()
{
    huart_.Instance = USART2;
    huart_.Init.BaudRate = baudrate_;
    huart_.Init.WordLength = UART_WORDLENGTH_8B;
    huart_.Init.StopBits = UART_STOPBITS_1;
    huart_.Init.Parity = UART_PARITY_NONE;
    huart_.Init.Mode = UART_MODE_TX_RX;
    huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart_.Init.OverSampling = UART_OVERSAMPLING_16;
    huart_.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart_.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    // TODO: add GPIO initialization for real hardware
    huart_.MspInitCallback = NULL;
    HAL_UART_Init(&huart_);
}

void hal::uart_stm32::putchar(char tx_char)
{
    HAL_UART_Transmit(&huart_, reinterpret_cast<uint8_t *>(&tx_char), 1,
                      HAL_MAX_DELAY);
}
