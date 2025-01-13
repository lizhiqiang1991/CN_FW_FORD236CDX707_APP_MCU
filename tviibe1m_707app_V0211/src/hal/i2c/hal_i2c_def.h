/***************************************************************************
*\file hal_i2c_def.h 
****************************************************************************/

#ifndef HAL_I2C_DEF_H
#define HAL_I2C_DEF_H

#if 0
typedef struct
{
    uint8_t u8Enable;
    uint8_t u8Width;
    uint8_t u8Status;
}i2c_fifo_config_t;
#endif

typedef struct
{
    uint8_t u8PinSCL;  
    uint8_t u8PinSDA;
    uint8_t u8RoleMode;
    uint8_t u8SlaveAddress;
    uint8_t u8SpeedMode;
    uint32_t u32ClockSpeed;
#if 0
    uint8_t u8SlaveEvent;
    uint8_t u8MasterEvent;
    i2c_fifo_config tsTxFifoConfig;
    i2c_fifo_config tsRxFifoConfig;
    uint8_t u8InterruptEnable;
    uint8_t u8NvicPriority;
#endif
    
    uint32_t u32Timeout;
}i2c_config_t;

typedef struct
{
    uint8_t u8SlaveAddress;
    uint8_t u8RegAddress;
    uint8_t u8MemAddressH;
    uint8_t u8MemAddressL;
    uint8_t* pData; 
    uint8_t u8Length;
}i2c_data_t;

/*u8RoleMode*/
#define I2C_MASTER_MODE     CY_SCB_I2C_MASTER
#define I2C_SLAVE_MODE      CY_SCB_I2C_SLAVE

/*u8SpeedMode*/
#define I2C_FAST_PLUS_MODE  100U   /**< 1000k bps*/
#define I2C_FAST_MODE       40U    /**< 400k bps*/
#define I2C_STANDARD_MODE   10U    /**< 100k bps*/

/*u32ClockSpeed*/
#define I2C_STANDARD_MODE_INCLK_FREQ    (2000000ul)  /**< 2Mhz*/  
#define I2C_FAST_MODE_INCLK_FREQ       (10000000ul) /**< 10Mhz*/
#define I2C_FAST_PLUS_MODE_INCLK_FREQ  (24000000ul) /**< 24Mhz*/

/*u8ReadWrite*/
#define I2C_ACTION_WRITE      CY_SCB_I2C_WRITE_XFER  
#define I2C_ACTION_READ       CY_SCB_I2C_READ_XFER  
          
/*High level Master mode API (pStatus) */
#define I2C_MASTER_NOT_READY    CY_SCB_I2C_MASTER_NOT_READY
#define I2C_MASTER_ADDR_NAK     CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK
#define I2C_MASTER_DATA_NAK     CY_SCB_I2C_MASTER_MANUAL_NAK
#define I2C_MASTER_ARB_LOST     CY_SCB_I2C_MASTER_MANUAL_ARB_LOST
#define I2C_MASTER_BUS_ERR      CY_SCB_I2C_MASTER_MANUAL_BUS_ERR
#define I2C_MASTER_ABORT_START  CY_SCB_I2C_MASTER_MANUAL_ABORT_START
#define I2C_MASTER_TIMEOUT      CY_SCB_I2C_MASTER_MANUAL_TIMEOUT

/*HAL_I2C_Master_DataByte_Read  (u32AckNack)  ACK/NACK*/
#define I2C_ACK     CY_SCB_I2C_ACK
#define I2C_NACK    CY_SCB_I2C_NAK

/*Redefine SCB GROUP because I2C, SPI, or UART are the same hardware unit*/
#define I2C_GROPU0  CYHAL_SCB0
#define I2C_GROPU1  CYHAL_SCB1
#define I2C_GROPU2  CYHAL_SCB2
#define I2C_GROPU3  CYHAL_SCB3
#define I2C_GROPU4  CYHAL_SCB4
#define I2C_GROPU5  CYHAL_SCB5
#define I2C_GROPU6  CYHAL_SCB6
#define I2C_GROPU7  CYHAL_SCB7

/*I2C SLAVE MODE */
typedef void(*I2C_ISR) (uint32_t u32IntFlag);

/*SLAVE MODE Event (u32IntFlag) */
/**
* Indicates the I2C hardware detected an error.
* Check \ref Cy_SCB_I2C_SlaveGetStatus to determine the source of the error.
*/
#define I2C_SLAVE_ERR_EVENT             CY_SCB_I2C_SLAVE_ERR_EVENT
/**
* Indicates that the slave was addressed and the master wants to read data.
* This event can be used to configure the slave Read buffer.
*/
#define I2C_SLAVE_READ_EVENT            CY_SCB_I2C_SLAVE_READ_EVENT
/**
* Indicates that the slave was addressed and the master wants to write data.
* This event can be used to configure the slave Write buffer.
*/
#define I2C_SLAVE_WRITE_EVENT           CY_SCB_I2C_SLAVE_WRITE_EVENT
/**
* All slave data from the configured Read buffer has been loaded into the
* TX FIFO. The content of the Read buffer can be modified. Applicable only
* if the TX FIFO is used.
*/
#define I2C_SLAVE_RD_IN_FIFO_EVENT      CY_SCB_I2C_SLAVE_RD_IN_FIFO_EVENT
/**
* The master has read all data out of the configured Read buffer.
* This event can be used to configure the next Read buffer. If the buffer
* remains empty, the \ref CY_SCB_I2C_DEFAULT_TX bytes are returned to the master.
*/
#define I2C_SLAVE_RD_BUF_EMPTY_EVENT    CY_SCB_I2C_SLAVE_RD_BUF_EMPTY_EVENT
/**
* Indicates the master completed reading from the slave (set by the master NAK
* or Stop)
*/
#define I2C_SLAVE_RD_CMPLT_EVENT        CY_SCB_I2C_SLAVE_RD_CMPLT_EVENT
/**
* Indicates the master completed writing to the slave (set by the master Stop
* or Restart)
*/
#define I2C_SLAVE_WR_CMPLT_EVENT        CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT

/*SLAVE MODE status (Cy_SCB_I2C_SlaveGetStatus)*/
#define I2C_SLAVE_STATUS_RD_BUSY       CY_SCB_I2C_SLAVE_RD_BUSY
#define I2C_SLAVE_STATUS_RD_IN_FIFO    CY_SCB_I2C_SLAVE_RD_IN_FIFO
#define I2C_SLAVE_STATUS_RD_CMPLT      CY_SCB_I2C_SLAVE_STATUS_RD_CMPLT
#define I2C_SLAVE_STATUS_RD_UNDRFL     CY_SCB_I2C_SLAVE_RD_UNDRFL
#define I2C_SLAVE_STATUS_WR_BUSY       CY_SCB_I2C_SLAVE_WR_BUSY
#define I2C_SLAVE_STATUS_WR_CMPLT      CY_SCB_I2C_SLAVE_WR_CMPLT
#define I2C_SLAVE_STATUS_WR_OVRFL      CY_SCB_I2C_SLAVE_WR_OVRFL
#define I2C_SLAVE_STATUS_ARB_LOST      CY_SCB_I2C_SLAVE_ARB_LOST
#define I2C_SLAVE_STATUS_BUS_ERR       CY_SCB_I2C_SLAVE_BUS_ERR


/*I2C master mode config*/
extern i2c_config_t gtsI2C_Master_Standard_Mode;  /**< 100k bps*/
extern i2c_config_t gtsI2C_Master_Fast_Mode;      /**< 400k bps*/
extern i2c_config_t gtsI2C_Master_FastPlus_Mode;  /**< 1000k bps*/

/*I2C slave mode config*/
extern i2c_config_t gtsI2C_Slave_Standard_Mode;  /**< 100k bps*/
extern i2c_config_t gtsI2C_Slave_Fast_Mode;      /**< 400k bps*/
extern i2c_config_t gtsI2C_Slave_FastPlus_Mode;  /**< 1000k bps*/

#endif

  