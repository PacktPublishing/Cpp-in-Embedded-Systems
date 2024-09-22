#include <uart_stm32.hpp>

hal::uart_stm32::uart_stm32(USART_TypeDef *inst)
    : instance_(inst)
{
}

void hal::uart_stm32::init(std::uint32_t baudrate)
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
    HAL_UART_Init(&huart_);
}

void hal::uart_stm32::write_array(const char * ptr, std::size_t len)
{
    // we must cast away costness due to ST HAL's API
    char * data_ptr = const_cast<char *>(ptr);
    HAL_UART_Transmit(&huart_, reinterpret_cast<uint8_t *>(data_ptr), len,
                      HAL_MAX_DELAY);
}