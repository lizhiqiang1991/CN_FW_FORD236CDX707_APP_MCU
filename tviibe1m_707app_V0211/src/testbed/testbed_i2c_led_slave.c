/*

File    : testbed_i2c_LED_slave.c
Purpose : i2c slave testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

#define USER_LED_1        KIT_LED_1_BLUE 
#define USER_LED_2        KIT_LED_4_BLUE 

/** Please Refer to datasheet to check the SCB5 SDA/SCL pin*/
#define I2C_PIN_SDA     KIT_I2C_SDA_PIN
#define I2C_PIN_SCL     KIT_I2C_SCL_PIN
#define I2C_SLAVE_SCB   KIT_I2C_SCB
 
/*7-bits slave address*/
#define I2C_SLAVE_DEVICE_ADDR  (0x08)

#define I2C_SLAVE_TXRX_BUF_SIZE 2    /*should be 2^n*/

/* Master read : Slave output data from g_i2c_tx_buf*/
static uint8_t g_i2c_tx_buf[I2C_SLAVE_TXRX_BUF_SIZE] = 
{
  /**
     data 0 : USER_LED_1 ,  0x00 (LED1 off)  0xaa (LED1 on)
     data 1 : USER_LED_2 ,  0x00 (LED2 off)  0xaa (LED2 on)
*/ 
  0x00u, /**< data 0*/
  0x00u  /**< data 1*/
};

/* Master write : Slave receive data put in to g_i2c_rx_buf*/
static uint8_t g_i2c_rx_buf[I2C_SLAVE_TXRX_BUF_SIZE];
static volatile uint8_t g_i2c_user_buf[I2C_SLAVE_TXRX_BUF_SIZE] = {0u};

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
  
  uint32_t u32RecvSize = 0u;
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
      printf("I2C_SLAVE_RD_CMPLT_EVENT 0x%x 0x%x\n",g_i2c_tx_buf[0],g_i2c_tx_buf[1]);
       
      /* Clear Read Buffer (use same buffer) */
      HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_SCB,&g_i2c_tx_buf[0u], I2C_SLAVE_TXRX_BUF_SIZE);
      break;
      
    case I2C_SLAVE_WR_CMPLT_EVENT:
      {
      printf("I2C_SLAVE_WR_CMPLT_EVENT\n");
      
      HAL_I2C_Slave_WriteTransferCount_Get(I2C_SLAVE_SCB, &u32RecvSize);
      
      printf("Recv Data --> ");
      for (uint32_t i = 0u ; i < u32RecvSize ; i++)
      {
        printf("0x%x ",g_i2c_rx_buf[i]);
      }
      printf("\n");
      
      // control LED1
      if (g_i2c_rx_buf[0u] == 0xaa || g_i2c_rx_buf[0u] == 0x00) /*LED1 ON/OFF*/
      {
        if (g_i2c_rx_buf[0u] == 0xaa)
          HAL_GPIO_PinWrite(USER_LED_1,PIN_HIGH);
        else
          HAL_GPIO_PinWrite(USER_LED_1,PIN_LOW);
        /* Update g_i2c_tx_buf for Master read*/
        g_i2c_tx_buf[0u] = g_i2c_rx_buf[0];
      }
      
      // control LED2
      if (g_i2c_rx_buf[1u] == 0xaa || g_i2c_rx_buf[1u] == 0x00) /*LED2 ON/OFF*/
      {
        if (g_i2c_rx_buf[1u] == 0xaa)
          HAL_GPIO_PinWrite(USER_LED_2,PIN_HIGH);
        else
          HAL_GPIO_PinWrite(USER_LED_2,PIN_LOW);
        /* Update g_i2c_tx_buf for Master read*/
        g_i2c_tx_buf[1u] = g_i2c_rx_buf[1u];
      }
       
      /* Clear Write Buffer */
      HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_SCB,&g_i2c_rx_buf[0u], I2C_SLAVE_TXRX_BUF_SIZE);
      }
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

void testbed_i2c_led_slave(void)
{
  __enable_irq();
  
  /*LEDs GPIO config as output*/
  HAL_GPIO_Pin_Init(USER_LED_1,&gtdPinOutputConfig);
  HAL_GPIO_Pin_Init(USER_LED_2,&gtdPinOutputConfig);
  HAL_GPIO_PinWrite(USER_LED_1,PIN_LOW);
  HAL_GPIO_PinWrite(USER_LED_2,PIN_LOW);
  
  printf("\n\nI2C LED Slave testbed\n");
  
  /** I2C Standard mode 100k bps*/
  gtsI2C_Slave_Standard_Mode.u8PinSDA = I2C_PIN_SDA;
  gtsI2C_Slave_Standard_Mode.u8PinSCL = I2C_PIN_SCL;
  gtsI2C_Slave_Standard_Mode.u8SlaveAddress = I2C_SLAVE_DEVICE_ADDR;
  
  uint8_t u8HalResult = HAL_I2C_Init( I2C_SLAVE_SCB, &gtsI2C_Slave_Standard_Mode);
  printf("I2C Slave mode Init = %d\n",u8HalResult);
  
  /* Master write : Slave receive data put in to g_i2c_rx_buf*/
  CY_ASSERT( HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_SCB,g_i2c_rx_buf, I2C_SLAVE_TXRX_BUF_SIZE) == DRIVER_TRUE);
  
  /* Master read : Slave output data from g_i2c_tx_buf*/
  CY_ASSERT( HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_SCB,g_i2c_tx_buf, I2C_SLAVE_TXRX_BUF_SIZE) == DRIVER_TRUE);
  
  CY_ASSERT( HAL_I2C_Callback_Register (I2C_SLAVE_SCB,Scb_I2C_Slave_Event) == DRIVER_TRUE);
  CY_ASSERT( HAL_I2C_Active( I2C_SLAVE_SCB , FUNC_ENABLE) == DRIVER_TRUE);
  Cy_SysLib_Delay(1300);
  
  /*
      MASTER writes 2 bytes to LED slave
      Slave address : W | SLAVE ACK | DATA[0] | SLAVE ACK | DATA[1] | SLAVE ACK| MASTER STOP 
    
      MASTER reads 2 bytes from LED slave
      Slave address : R | SLAVE ACK | DATA[0] | MASTER ACK | DATA[1] | MASTER NACK | MASTER STOP
  */
  for(;;)
  {
    ;
  }
}