#include <adc_stm32.hpp>

void hal::adc_stm32::init() {
    ADC_ChannelConfTypeDef sConfig = {0};

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    adc_handle_.Instance = ADC1;
    adc_handle_.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    adc_handle_.Init.Resolution = ADC_RESOLUTION_12B;
    adc_handle_.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle_.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    adc_handle_.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adc_handle_.Init.LowPowerAutoWait = DISABLE;
    adc_handle_.Init.LowPowerAutoPowerOff = DISABLE;
    adc_handle_.Init.ContinuousConvMode = DISABLE;
    adc_handle_.Init.DiscontinuousConvMode = DISABLE;
    adc_handle_.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_handle_.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_handle_.Init.DMAContinuousRequests = DISABLE;
    adc_handle_.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    if (HAL_ADC_Init(&adc_handle_) != HAL_OK)
    {
        //Error_Handler();
    }

    /** Configure for the selected ADC regular channel to be converted.
    */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&adc_handle_, &sConfig) != HAL_OK)
    {
        //Error_Handler();
    }
}
        
std::expected<units::voltage, hal::adc::error> hal::adc_stm32::get_reading() {
    HAL_ADC_Start(&adc_handle_);
    if(HAL_ADC_PollForConversion(&adc_handle_, 1000) != HAL_OK) {
        return std::unexpected(hal::adc::error::timeout);
    }

    auto adc_val = HAL_ADC_GetValue(&adc_handle_);

    return ref_voltage_ * adc_val / 4096.f;
}