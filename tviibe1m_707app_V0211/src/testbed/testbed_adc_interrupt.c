/*

File    : testbed_adc_interrupt.c
Purpose : ADC testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>

#define ADC_LOGIC_CH  ADC0_LOGIC_CH20

static void ADC0_Callback (bool u8GroupDone)
{
    uint16_t         u16Result = 0u;
    adc_ch_stattus_t statusBuff ;
    if (true == u8GroupDone)
    {
        /* Get the result(s) */
        HAL_ADC_Channel_Result_Get (ADC_LOGIC_CH,&u16Result, &statusBuff);
        if (true == statusBuff.valid)
        {
            float v = (5.0 *((float)u16Result / 4095.0));
            /* Display ADC result */
            printf("ADC result = %04d %.3fV\n", u16Result,v);
        }
    }
}

void testbed_adc_interrupt(void) 
{
    __enable_irq();
    
    printf("HAL ADC Single channel (interrupt) testing\n");
      
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
    
    /* Initialize ADC Ch (ADC_LOGIC_CH) and Enable */
    /*Specify ADC channel pin*/
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_AN0; //ADC_PIN_VCCD; //ADC_PIN_AN0;
    
    /**可設定Channel自動連續觸發 會看到 ADC0_Callback 不停進入 PRINTF 印很快*/
    //gtdADC_ChannelConfig.u8TriggerSelection = ADC_TRIGGER_CONTINUOUS;
    
     /** 指定 ADC0 腳位 ADC_PIN_AN0 到 LOGIC CHANNEL 0*/
    CY_ASSERT( HAL_ADC_Channel_Init   (ADC_LOGIC_CH, &gtdADC_ChannelConfig) == DRIVER_TRUE);
    CY_ASSERT( HAL_ADC_Channel_Active (ADC_LOGIC_CH, FUNC_ENABLE) == DRIVER_TRUE);
    
    /* Register adc interrupt*/
    CY_ASSERT( HAL_ADC_Callback_Register (ADC_LOGIC_CH, ADC0_Callback) == DRIVER_TRUE);
      
    /* Issue SW trigger A/D conversion */
    CY_ASSERT( HAL_ADC_Channel_SW_Trigger(ADC_LOGIC_CH) == DRIVER_TRUE);  
    
    for (int i = 0 ; i < 10 ; i++)
    { 
        /* Trigger next conversion */
        /** 如果設定 ADC_TRIGGER_CONTINUOUS 不需要每次執行 HAL_ADC_Channel_SW_Trigger*/
        if (ADC_TRIGGER_OFF == gtdADC_ChannelConfig.u8TriggerSelection)
            HAL_ADC_Channel_SW_Trigger(ADC_LOGIC_CH);
        
        /*如果設定 ADC_TRIGGER_OFF 每 1.2 秒 SW Trigger 一次進行 ADC 取樣*/
        Cy_SysLib_Delay(1200);
    }
    
    while(1);;
    //CY_ADC_POT_IRQN
}