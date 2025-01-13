/***************************************************************************
*\file hal_uart_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_uart_def.h"
#include "hal_gpio_def.h"

/*UART 115200,N,8,1 Configuration*/
uart_config_t gts_UART_115200_bps_Config = 
{
  .u8TxPin  = IO_NC,                     /**< UART TX pin*/
  .u8RxPin  = IO_NC,                     /**< UART RX pin*/
  .u8CtsPin = IO_NC,                     /**< UART CTS pin*/
  .u8RtsPin = IO_NC,                     /**< UART RTS pin*/
  .u32BaudRate = UART_BAUD_RATE_115200,  /**< 4,000,000 ~ 9,600 bps*/
  .u8Parity    = UART_PARITY_NONE,       /**< PARITY_NONE UART has no parity check.
                                              PARITY_EVEN UART has even parity check.
                                              PARITY_ODD UART has odd parity check.*/
   .u8DataBits = UART_DATA_BITS_8,       /**< The number of data bits (generally 8 or 9)*/
   .u8StopBits = UART_STOP_BITS_1,       /**< The number of stop bits (generally 0 or 1)*/
};


/*** End of file ***/

  