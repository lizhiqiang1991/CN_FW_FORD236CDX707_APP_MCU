/*

File    : testbed_i2c_slave.c
Purpose : i2c slave testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

#define USER_LED        KIT_LED_1_BLUE 

/** Please Refer to datasheet to check the SCB5 SDA/SCL pin*/
#define I2C_PIN_SDA     KIT_I2C_SDA_PIN
#define I2C_PIN_SCL     KIT_I2C_SCL_PIN
#define I2C_SLAVE_SCB   KIT_I2C_SCB
 
/*7-bits slave address*/
#define I2C_SLAVE_DEVICE_ADDR  (0x08)

#define I2C_SLAVE_TXRX_BUF_SIZE 8 /*should be 2^n*/

/* Master read : Slave output data from g_i2c_tx_buf*/
static uint8_t g_i2c_tx_buf[I2C_SLAVE_TXRX_BUF_SIZE] = 
{
  0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08
};

/* Master write : Slave receive data put in to g_i2c_rx_buf*/
static uint8_t g_i2c_rx_buf[I2C_SLAVE_TXRX_BUF_SIZE];

static void SlaveErrorEvent (uint32_t u32ErrStatus)
{
  printf("I2C_SLAVE_ERR_EVENT : Error Code : 0x%x\n",u32ErrStatus);
  if (u32ErrStatus & I2C_SLAVE_STATUS_RD_BUSY)
  {
    printf("\tI2C_SLAVE_STATUS_RD_BUSY\n");
  }
  if (u32ErrStatus & I2C_SLAVE_STATUS_RD_UNDRFL)
  {
    printf("\tI2C_SLAVE_STATUS_RD_UNDRFL\n");
  }
  
  if (u32ErrStatus & I2C_SLAVE_STATUS_WR_BUSY  )
  {
    printf("\tI2C_SLAVE_STATUS_WR_BUSY\n");
  }
  
  if (u32ErrStatus & I2C_SLAVE_STATUS_WR_OVRFL )
  {
    printf("\tI2C_SLAVE_STATUS_WR_OVRFL\n");
  }
  if (u32ErrStatus & I2C_SLAVE_STATUS_ARB_LOST )
  {
    printf("\tI2C_SLAVE_STATUS_ARB_LOST\n");
  }
  if (u32ErrStatus & I2C_SLAVE_STATUS_BUS_ERR  )
  {
    printf("\tI2C_SLAVE_STATUS_BUS_ERR\n");
  }
}

static void Scb_I2C_Slave_Event(uint32_t u32Events)
{
  HAL_GPIO_PinWrite(USER_LED,PIN_LOW);
  uint32_t u32RecvSize = 0;
  switch (u32Events)
  {
    case I2C_SLAVE_READ_EVENT:
      printf("I2C_SLAVE_READ_EVENT\n");
      break;
      
    case I2C_SLAVE_WRITE_EVENT:
      printf("I2C_SLAVE_WRITE_EVENT\n");
      break;
      
    case I2C_SLAVE_RD_IN_FIFO_EVENT:
      printf("I2C_SLAVE_RD_IN_FIFO_EVENT\n");
      break;
      
    case I2C_SLAVE_RD_BUF_EMPTY_EVENT:
      printf("I2C_SLAVE_RD_BUF_EMPTY_EVENT\n");
      break;
      
    case I2C_SLAVE_RD_CMPLT_EVENT:
      printf("I2C_SLAVE_RD_CMPLT_EVENT\n");
      /* Clear Read Buffer (use same buffer) */
      HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_SCB,&g_i2c_tx_buf[0], I2C_SLAVE_TXRX_BUF_SIZE);
      break;
      
    case I2C_SLAVE_WR_CMPLT_EVENT:
      printf("I2C_SLAVE_WR_CMPLT_EVENT\n");
      HAL_I2C_Slave_WriteTransferCount_Get(I2C_SLAVE_SCB, &u32RecvSize);
      /* Printf Recv Data  */
      printf("Recv Data --> ");
      for (uint32_t i = 0 ; i < u32RecvSize ; i++)
      {
        printf("0x%x ",g_i2c_rx_buf[i]);
      }
      printf("\n");
      
      /* Clear Write Buffer */
      HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_SCB,&g_i2c_rx_buf[0], I2C_SLAVE_TXRX_BUF_SIZE);
      break;
    case I2C_SLAVE_ERR_EVENT:
      {
      uint32_t u32ErrStatus;
      HAL_I2C_Slave_Status_Get(I2C_SLAVE_SCB,&u32ErrStatus);
      SlaveErrorEvent(u32ErrStatus);
      }
      break;
    default:
      break;
  }
}

void testbed_i2c_slave(void)
{
  __enable_irq();
  
  /*LED GPIO config as output*/
  HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
  
  printf("\n\nI2C Slave testbed\n");
  
  /** I2C Standard mode 100k bps*/
  gtsI2C_Slave_Standard_Mode.u8PinSDA = I2C_PIN_SDA;
  gtsI2C_Slave_Standard_Mode.u8PinSCL = I2C_PIN_SCL;
  gtsI2C_Slave_Standard_Mode.u8SlaveAddress = I2C_SLAVE_DEVICE_ADDR;
  
  uint8_t u8HalResult = HAL_I2C_Init( I2C_SLAVE_SCB, &gtsI2C_Slave_Standard_Mode);
  printf("I2C Slave mode Init = %d\n",u8HalResult);
  
  /* Master write : Slave receive data put in to g_i2c_rx_buf*/
  CY_ASSERT( HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_SCB,&g_i2c_rx_buf[0], I2C_SLAVE_TXRX_BUF_SIZE) == DRIVER_TRUE);
  
  /* Master read : Slave output data from g_i2c_tx_buf*/
  CY_ASSERT( HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_SCB,&g_i2c_tx_buf[0], I2C_SLAVE_TXRX_BUF_SIZE) == DRIVER_TRUE);
  
  CY_ASSERT( HAL_I2C_Callback_Register (I2C_SLAVE_SCB,Scb_I2C_Slave_Event) == DRIVER_TRUE);
  CY_ASSERT( HAL_I2C_Active( I2C_SLAVE_SCB , FUNC_ENABLE) == DRIVER_TRUE);
  Cy_SysLib_Delay(1300);
  
  HAL_GPIO_PinWrite(USER_LED,PIN_HIGH);
  for(;;)
  {
    ;
  }
}