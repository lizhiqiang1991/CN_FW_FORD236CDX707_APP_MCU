/**
* @file UartApp.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-03-23
* 
* @copyright Copyright (c) 2021
* 
*/

#include "UartApp.h"


void UartApp_initialize(void)
{
    
    gts_UART_115200_bps_Config.u8TxPin = PIN_UART_SCB7_TX;
    gts_UART_115200_bps_Config.u8RxPin = PIN_UART_SCB7_RX;
    
    CY_ASSERT( HAL_Uart_Init(UART_GROPU7,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_GROPU7,FUNC_ENABLE) == DRIVER_TRUE);
    
}