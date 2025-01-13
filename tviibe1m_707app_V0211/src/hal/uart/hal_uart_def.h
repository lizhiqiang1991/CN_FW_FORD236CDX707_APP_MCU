/***************************************************************************
*\file hal_uart_def.h 
****************************************************************************/

#ifndef HAL_UART_DEF_H
#define HAL_UART_DEF_H

/*UART Configuration*/
typedef struct
{
    uint8_t u8TxPin;            /**< UART TX pin*/
    uint8_t u8RxPin;            /**< UART RX pin*/
    uint8_t u8CtsPin;           /**< UART CTS pin*/
    uint8_t u8RtsPin;            /**< UART RTS pin*/
    
    uint32_t u32BaudRate;       /**< 4,000,000 ~ 9,600 bps*/
    uint8_t  u8Parity;          /**< PARITY_NONE UART has no parity check.
                                     PARITY_EVEN UART has even parity check.
                                     PARITY_ODD UART has odd parity check.*/
    
    uint8_t  u8DataBits;        /**< The number of data bits (generally 8 or 9)*/
    uint8_t  u8StopBits;        /**< The number of stop bits (generally 0 or 1)*/
}uart_config_t;

typedef void(*UART_ISR) (uint32_t u32Event);

/*u32BaudRate*/
#define UART_BAUD_RATE_4M       (4000000U) /**<      4M bps*/
#define UART_BAUD_RATE_2M       (2000000U) /**<      2M bps*/
#define UART_BAUD_RATE_1M       (1000000U) /**<      1M bps*/
#define UART_BAUD_RATE_500K      (500000U) /**<    500K bps*/
#define UART_BAUD_RATE_468000    (460800U) /**<    468K bps*/
#define UART_BAUD_RATE_250000    (250000U) /**<    250K bps*/
#define UART_BAUD_RATE_230400    (230400U) /**<  230.4K bps*/
#define UART_BAUD_RATE_153600    (153600U) /**<  153.6K bps*/
#define UART_BAUD_RATE_125000    (125000U) /**<    125K bps*/
#define UART_BAUD_RATE_115200    (115200U) /**<  115.2K bps*/
#define UART_BAUD_RATE_76800      (76800U) /**<   76.8K bps*/
#define UART_BAUD_RATE_57600      (57600U) /**<   57.6K bps*/
#define UART_BAUD_RATE_38400      (38400U) /**<   38.4K bps*/
#define UART_BAUD_RATE_28800      (28800U) /**<   28.8K bps*/
#define UART_BAUD_RATE_19200      (19200U) /**<   19.2K bps*/
#define UART_BAUD_RATE_10417      (10417U) /**<   10417 bps*/
#define UART_BAUD_RATE_9600        (9600U) /**<    9600 bps*/

/*u8Parity*/
#define UART_PARITY_NONE       CY_SCB_UART_PARITY_NONE
#define UART_PARITY_EVEN       CY_SCB_UART_PARITY_EVEN
#define UART_PARITY_ODD        CY_SCB_UART_PARITY_ODD 

/*u8DataBits*/
#define UART_DATA_BITS_8       (8U)
#define UART_DATA_BITS_9       (9U)

/*u8StopBits*/
#define UART_STOP_BITS_1       CY_SCB_UART_STOP_BITS_1   
#define UART_STOP_BITS_1_5     CY_SCB_UART_STOP_BITS_1_5 
#define UART_STOP_BITS_2       CY_SCB_UART_STOP_BITS_2   
#define UART_STOP_BITS_2_5     CY_SCB_UART_STOP_BITS_2_5 
#define UART_STOP_BITS_3       CY_SCB_UART_STOP_BITS_3   
#define UART_STOP_BITS_3_5     CY_SCB_UART_STOP_BITS_3_5 
#define UART_STOP_BITS_4       CY_SCB_UART_STOP_BITS_4   

/*Redefine SCB GROUP because I2C, SPI, or UART are the same hardware unit*/
#define UART_GROPU0  CYHAL_SCB0
#define UART_GROPU1  CYHAL_SCB1
#define UART_GROPU2  CYHAL_SCB2
#define UART_GROPU3  CYHAL_SCB3
#define UART_GROPU4  CYHAL_SCB4
#define UART_GROPU5  CYHAL_SCB5
#define UART_GROPU6  CYHAL_SCB6
#define UART_GROPU7  CYHAL_SCB7

/*u32Event*/
/**
* All data elements specified by \ref Cy_SCB_UART_Transmit have been loaded
* into the TX FIFO
*/
#define UART_TRANSMIT_IN_FIFO_EVENT    CY_SCB_UART_TRANSMIT_IN_FIFO_EVENT

/** The transmit operation started by \ref Cy_SCB_UART_Transmit is complete */
#define UART_TRANSMIT_DONE_EVENT       CY_SCB_UART_TRANSMIT_DONE_EVENT    

/** The receive operation started by \ref Cy_SCB_UART_Receive is complete */
#define UART_RECEIVE_DONE_EVENT        CY_SCB_UART_RECEIVE_DONE_EVENT      

/**
* The ring buffer is full, there is no more space for additional data.
* Additional data is stored in the RX FIFO until it becomes full, at which
* point data is dropped.
*/
#define UART_RB_FULL_EVENT            CY_SCB_UART_RB_FULL_EVENT        

/**
* An error was detected during the receive operation. This includes overflow,
* frame error, or parity error. Check \ref Cy_SCB_UART_GetReceiveStatus to
* determine the source of the error.
*/
#define UART_RECEIVE_ERR_EVENT        CY_SCB_UART_RECEIVE_ERR_EVENT

/**
* An error was detected during the transmit operation. This includes a NACK
* or lost arbitration. Check \ref Cy_SCB_UART_GetTransmitStatus to determine
* the source of the error
*/
#define UART_TRANSMIT_ERR_EVENT        CY_SCB_UART_TRANSMIT_ERR_EVENT

/*UART config*/
extern uart_config_t gts_UART_115200_bps_Config;

#endif

  