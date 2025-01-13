/**
* @file UartApp.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-03-23
* 
* @copyright Copyright (c) 2021
* 
*/
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"


//#define TX_PIN_UART     IO_P13_1
//#define RX_PIN_UART     IO_P13_0
#define TX_PIN_UART     IO_P2_1
#define RX_PIN_UART     IO_P2_0
#define UART_SCB        UART_GROPU7
#define DEBUG_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)


void UartApp_initialize(void);

