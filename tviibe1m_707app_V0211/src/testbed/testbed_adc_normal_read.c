/*

File    : testbed_adc_normal_read.c
Purpose : ADC testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>

void testbed_adc_normal_read(void) 
{
    __enable_irq();
     
    printf("HAL ADC Single channel testing\n");
     
    /*Specify ADC channel pin*/
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_AN0;
    //gtdADC_ChannelConfig.u8TriggerSelection = ADC_TRIGGER_CONTINUOUS;
    
#if 0
    adc_channel_config_t tsChannelConfig = 
    {
        .u8PinAddress = ADC_PIN_AN0, /** channel 0*/
        .u8IsGroupEnd = true,                 /**< This value indicates that this channel is the last channel of a group or not */
        .u8SignExtention = ADC_SIGN_EXTENTION_UNSIGNED,              /** Select whether result data is signed or unsigned. */
        .u8RangeDetectionMode = ADC_RANGE_DETECTION_MODE_INSIDE_RANGE,         /**< Select range detection mode*/
        .u16RangeDetectionLoThreshold = 0x0000u, /**< Range detect low threshold (Lo) */
        .u16RangeDetectionHiThreshold = 0x0FFFu, /**< Range detect high threshold (Hi) */
    };
#endif
    
    /* Initialize ADC0 Group*/
    CY_ASSERT( HAL_ADC_Init(ADC_GROPU0,&gtdADC_Config) == DRIVER_TRUE);
    
    /* Initialize ADC Ch (ADC0_LOGIC_CH0) and Enable */
    /* 指定 ADC0 腳位 ADC_PIN_AN0 到 ADC0 [LOGIC CHANNEL 0 ]*/
    CY_ASSERT( HAL_ADC_Channel_Init   (ADC0_LOGIC_CH0, &gtdADC_ChannelConfig) == DRIVER_TRUE);
    CY_ASSERT( HAL_ADC_Channel_Active (ADC0_LOGIC_CH0, FUNC_ENABLE) == DRIVER_TRUE);
    
    /* Issue SW trigger A/D conversion */
    CY_ASSERT( HAL_ADC_Channel_SW_Trigger(ADC0_LOGIC_CH0) == DRIVER_TRUE);  
    
    uint16_t         u16Result = 0u;
    adc_ch_stattus_t statusBuff ;
    
    for (;;)
    {
        /* Get the result(s) */
        HAL_ADC_Channel_Result_Get (ADC0_LOGIC_CH0,&u16Result, &statusBuff);
       
        if (statusBuff.valid == true)
        {
            float v = (5.0 *((float)u16Result / 4095.0));
            /* Display ADC result */
            printf("ADC result = %04d %.3fV\n", u16Result,v);
        }
        Cy_SysLib_Delay(20);
        
        /* Trigger next conversion */
        /** 如果設定 ADC_TRIGGER_CONTINUOUS 不需要每次執行 HAL_ADC_Channel_SW_Trigger*/
        if (ADC_TRIGGER_OFF == gtdADC_ChannelConfig.u8TriggerSelection)
            HAL_ADC_Channel_SW_Trigger(ADC0_LOGIC_CH0);
        
        Cy_SysLib_Delay(1200);
    }
    
    //CY_ADC_POT_IRQN
}