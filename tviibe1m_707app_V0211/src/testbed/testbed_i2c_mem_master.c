/*

File    : testbed_i2c_master.c
Purpose : i2c master testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the SCB5 SDA/SCL pin*/
#define I2C_PIN_SDA     KIT_I2C_SDA_PIN
#define I2C_PIN_SCL     KIT_I2C_SCL_PIN
#define I2C_MASTER_SCB  KIT_I2C_SCB
 
/*7-bits slave address*/
#define DEVICE_ADDR  (0x16)

static void i2c_mem_api_test(void)
{
  /** HAL_I2C_Master_Reg_Transmit / HAL_I2C_Master_Reg_Receive*/
  uint8_t u8RegData  = 0xaau;
  uint32_t u32Status = 0u;
  i2c_data_t i2c_tx_pak = 
  {
    .u8SlaveAddress = DEVICE_ADDR,
    .u8MemAddressH = 0x00,
    .u8MemAddressL = 0x00,
    .pData = &u8RegData,
    .u8Length = 1u,
  };

  CY_ASSERT ( HAL_I2C_Master_Mem_Transmit(I2C_MASTER_SCB,&i2c_tx_pak,&u32Status) == DRIVER_TRUE);
  printf("HAL_I2C_Master_Mem_Transmit address 0 = 0x%x\n",u8RegData);
  
  /*Read 4 bytes from Memory address 0*/
  uint8_t rx_data[4] = {0,0,0,0};
  i2c_data_t i2c_rx_pak = 
  {
    .u8SlaveAddress = DEVICE_ADDR,
    .u8MemAddressH = 0x00,
    .u8MemAddressL = 0x00,
    .pData = rx_data,
    .u8Length = sizeof(rx_data),
  };
  
 /*clear data*/
  CY_ASSERT( HAL_I2C_Master_Mem_Receive(I2C_MASTER_SCB,&i2c_rx_pak,&u32Status) == DRIVER_TRUE);
  printf("HAL_I2C_Master_Mem_Receive data --> 0x%x 0x%x 0x%x 0x%x \n\n",rx_data[0],rx_data[1],rx_data[2],rx_data[3]); /**<-- u8RegData should be 0x55*/
  
  Cy_SysLib_Delay(1300);
}

void testbed_i2c_mem_master(void)
{
  __enable_irq();
  
  printf("\n\nI2C Master Mem_Transmit/Receive testbed\n");
  
  /** I2C Standard mode 100k bps*/
  gtsI2C_Master_Standard_Mode.u8PinSDA = I2C_PIN_SDA;
  gtsI2C_Master_Standard_Mode.u8PinSCL = I2C_PIN_SCL;
  uint8_t u8HalResult = HAL_I2C_Init( I2C_MASTER_SCB, &gtsI2C_Master_Standard_Mode);
  printf("I2C Master Init = %d\n",u8HalResult);
  HAL_I2C_Active( I2C_MASTER_SCB , FUNC_ENABLE);
  
  /*High level I2C API*/ 
  i2c_mem_api_test();
  
  for(;;);
}