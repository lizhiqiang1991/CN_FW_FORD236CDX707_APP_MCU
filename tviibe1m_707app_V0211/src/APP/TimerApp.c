/**
* @file TimerApp.c
* @author Derek Wen
* @brief 
* @version 0.1
* @date 2021-02-02
* 
* @copyright Copyright (c) 2021
* 
*/
/* Include standard */
#include <stdio.h>
#include <stdint.h>

/* include HAL */
#include "TimerApp.h"


#if 0
void LED_Timer_Handler (void* context)
{
    
    
}
#endif

void Timer_Initialize(void)
{
    /* Create Timer for 1ms */
    HAL_Timer_Init(TIMER_1MINISEC,&gts_1mSec_Timer_Continuous_Config);
    /* Create Timer for 10ms */
    HAL_Timer_Init(TIMER_10MINISEC,&gts_10mSec_Timer_Continuous_Config);  //10ms
    /* Refuster Callback */
    // HAL_Timer_Callback_Register (TIMER_1MINISEC,LED_Timer_Handler,NULL);
    /* Active Timer */
    HAL_Timer_Active(TIMER_1MINISEC         ,FUNC_ENABLE);
    HAL_Timer_Active(TIMER_10MINISEC      ,FUNC_ENABLE);
    /* Start Tiemr */
    HAL_Timer_Start(TIMER_1MINISEC          ,FUNC_ENABLE);
    HAL_Timer_Start(TIMER_10MINISEC       ,FUNC_ENABLE);
}

#if 0
void Timer_1msProcess_Main (void)
{
    if(HAL_Timer_Flag_Get(TIMER_1MINISEC) == DRIVER_TRUE)
    {
        /* 1ms Process Start*/
        HAL_GPIO_PinToggle(PIN_PANEL_FPCA_DET_OUT_R);
        
        
        /* 1ms Process End */
        (void)HAL_Timer_Flag_Clear(TIMER_1MINISEC);
    }
}
#endif