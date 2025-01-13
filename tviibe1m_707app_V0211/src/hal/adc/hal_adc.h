/***************************************************************************
*\file hal_adc.h 
****************************************************************************/

#ifndef HAL_ADC_H
#define HAL_ADC_H

uint8_t HAL_ADC_Init    (uint8_t u8ADCGroup, adc_group_config_t* tsConfig);
uint8_t HAL_ADC_DeInit  (uint8_t u8ADCGroup);

/**< uint8_t  u8ADC_Channel; For example : ADC0_LOGIC_CH2 , means adc[0] logic channel 2*/
uint8_t HAL_ADC_Channel_Init       (uint8_t u8ADC_Channel, adc_channel_config_t* tsChannelConfig);

uint8_t HAL_ADC_Channel_Active     (uint8_t u8ADC_Channel, uint8_t u8Enable);
uint8_t HAL_ADC_Channel_SW_Trigger (uint8_t u8ADC_Channel);
uint8_t HAL_ADC_Channel_Result_Get (uint8_t u8ADC_Channel, uint16_t* pResult, adc_ch_stattus_t* tsStatus);

uint8_t HAL_ADC_Callback_Register  (uint8_t u8ADC_Channel, HAL_ADC_CALLBACK pCallback);
 
#endif

  