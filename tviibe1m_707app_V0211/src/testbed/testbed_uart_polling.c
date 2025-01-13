/*

File    : testbed_i2c_master.c
Purpose : i2c master testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the SCB3 UART TX/RX pin*/
#define TX_PIN_UART     IO_P13_1
#define RX_PIN_UART     IO_P13_0
#define UART_SCB        (3u)

#if 0
void Term_Printf(void *fmt, ...)
{
    uint8_t uart_out_data[128u];
    va_list arg;

    /* UART Print */
    va_start(arg, fmt);
    vsprintf((char*)&uart_out_data[0], (char*)fmt, arg);
    while (Cy_SCB_UART_IsTxComplete(CY_USB_SCB_TYPE) != true) {};
    Cy_SCB_UART_PutArray(CY_USB_SCB_TYPE, uart_out_data, strlen((char *)uart_out_data));
    va_end(arg);
}
#endif

static const char* pString = "\n\nUART Polling testbed\n";

void testbed_uart_polling(void)
{
  __enable_irq();
  gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
  gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
  
  CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
  CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
                             
  HAL_Uart_Write(UART_SCB,(void*)pString,strlen(pString));
  
  
  for(;;);
}