/*

File    : testbed_uart_interrupt.c
Purpose : uart testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     IO_P0_1 /*EVB on board USB to UART*/  //IO_P13_1 
#define RX_PIN_UART     IO_P0_0                               //IO_P13_0
#define UART_SCB        UART_GROPU0                           //(3u)

/* vt100 command */
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define GREEN "\033[0;32;32m"
#define YELLOW "\033[1;33m"
#define WHITE "\033[1;37m"

#define CONSOLE_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)
#define GET_CHAR(x) HAL_Uart_Getc(UART_SCB,(uint8_t*)x)
#define PUT_CHAR(x) HAL_Uart_Putc(UART_SCB,x)

static uint8_t u8Data;

static void Isr_scb_uart (uint32_t u32Event)
{
    switch (u32Event)
    {
      case UART_RECEIVE_DONE_EVENT:
        {
        //GET_CHAR(&cTemp);	
        PUT_CHAR(u8Data);
        }
        break;
    }
}


void testbed_uart_interrupt(void)
{
    __enable_irq();
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    CONSOLE_PF ("\033[1J" "\033[0;0H");
    CONSOLE_PF ("[TEST]:>");
    HAL_Uart_Callback_Register(UART_SCB,Isr_scb_uart);
        
#if 1
    uint32_t stt;
    for (;;)
    {
         /* Start receiving */
        UART_RECV(UART_SCB, (char*)&u8Data, 1);
        
        do {
            
             HAL_Uart_GetRxStatus(UART_SCB,&stt);
        }while ((stt & CY_SCB_UART_RECEIVE_ACTIVE) ==  CY_SCB_UART_RECEIVE_ACTIVE);
       
    }
    
#else    
    uint16_t u16NumInRx = 0;
    for (;;)
    {
        char cTemp = 0x00;
        HAL_Uart_GetNumInRxFifo(UART_SCB,&u16NumInRx);
        if (u16NumInRx != 0)
        {
            GET_CHAR(&cTemp);
            PUT_CHAR(cTemp);
        }
        Cy_SysLib_Delay(1);
    }
#endif        
}