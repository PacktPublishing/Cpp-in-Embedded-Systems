#include <uart_stm32.hpp>

hal::uart_stm32::uart_stm32(USART_TypeDef *inst, std::uint32_t baud)
    : instance(inst), baudrate(baud)
{
}

void hal::uart_stm32::init()
{
    huart.Instance = USART2;
    huart.Init.BaudRate = 115200;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Parity = UART_PARITY_NONE;
    huart.Init.Mode = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;
    huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    // huart.MspInitCallback = mspInitCallback;

    if(HAL_UART_Init(&huart) != HAL_OK)
    {
        // Error_Handler();
    }
}

void hal::uart_stm32::putchar(char c)
{
    HAL_UART_Transmit(&huart, reinterpret_cast<uint8_t *>(&c), 1,
                      HAL_MAX_DELAY);
}
