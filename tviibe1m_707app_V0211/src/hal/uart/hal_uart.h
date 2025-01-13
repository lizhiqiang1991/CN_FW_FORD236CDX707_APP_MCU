/***************************************************************************
*\file hal_uart.h 
****************************************************************************/

#ifndef HAL_UART_H
#define HAL_UART_H

uint8_t HAL_Uart_Init   (uint8_t u8UartGroup, uart_config_t* tsConfig);
uint8_t HAL_Uart_DeInit (uint8_t u8UartGroup);
uint8_t HAL_Uart_Active (uint8_t u8UartGroup,uint8_t u8Enable);
uint8_t HAL_Uart_Write  (uint8_t u8UartGroup,uint8_t* pBuf,uint16_t u16Len);
uint8_t HAL_Uart_Read   (uint8_t u8UartGroup,uint8_t* pBuf,uint16_t u16Len);
uint8_t HAL_Uart_Getc	(uint8_t u8UartGroup,uint8_t* pu8Char);		
uint8_t HAL_Uart_Putc	(uint8_t u8UartGroup,uint8_t u8Value);
uint8_t HAL_Uart_IsTxComplete   (uint8_t u8UartGroup,bool* pTxComplete);
uint8_t HAL_Uart_GetNumInRxFifo (uint8_t u8UartGroup,uint16_t* pu16Rxnum);

uint8_t HAL_Uart_GetRxStatus(uint8_t u8UartGroup,uint32_t* pRxStatus);
uint8_t HAL_Uart_GetTxStatus(uint8_t u8UartGroup,uint32_t* pTxStatus);

uint8_t HAL_Uart_Callback_Register (uint8_t u8UartGroup, UART_ISR pUartCallback);
uint8_t HAL_Uart_Interrupt_Set(uint8_t u8UartGroup, uint8_t u8Enable, uint8_t u8Priority);

uint8_t HAL_UartPrintf(uint8_t u8UartGroup,void *fmt, ...);

void UART_RECV(uint8_t u8UartGroup, char* buf,uint16_t len);

#endif

  