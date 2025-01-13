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
#define DEVICE_ADDR  (0x28)

static void low_level_i2c_api_test(void)
{
  uint8_t u8Data = 0xaa;
  uint32_t u32Status = 0;
  /*Write to device address 0x28 , register 0 , data 0xaa*/
  /*Start bit*/
  CY_ASSERT( HAL_I2C_Master_Start_Send(I2C_MASTER_SCB,DEVICE_ADDR,I2C_ACTION_WRITE) == DRIVER_TRUE);
  /*Set register 0*/
  CY_ASSERT( HAL_I2C_Master_DataByte_Write(I2C_MASTER_SCB,0x00,&u32Status) == DRIVER_TRUE);
  /*Write to register 0 data 0xaa*/
  CY_ASSERT( HAL_I2C_Master_DataByte_Write(I2C_MASTER_SCB,u8Data,&u32Status) == DRIVER_TRUE);
  /*Write Stop bit*/
  CY_ASSERT( HAL_I2C_Master_Stop_Send(I2C_MASTER_SCB) == DRIVER_TRUE);
  printf("Write Register 0 = 0x%x\n",u8Data);
   
  Cy_SysLib_Delay(300);
  
  /*Read it back device address 0x28 , register 0*/
  CY_ASSERT( HAL_I2C_Master_Start_Send(I2C_MASTER_SCB,DEVICE_ADDR,I2C_ACTION_WRITE) == DRIVER_TRUE);
  /*Set register 0*/
  CY_ASSERT( HAL_I2C_Master_DataByte_Write(I2C_MASTER_SCB,0x00,&u32Status) == DRIVER_TRUE);
  /*Send restart bit*/
  CY_ASSERT( HAL_I2C_Master_ReStart_Send (I2C_MASTER_SCB,DEVICE_ADDR,I2C_ACTION_READ) == DRIVER_TRUE);
  /*Read 1 byte , master response NACK*/
  CY_ASSERT( HAL_I2C_Master_DataByte_Read(I2C_MASTER_SCB,&u8Data,I2C_NACK,&u32Status) == DRIVER_TRUE);
  /*Send Read Stop*/
  CY_ASSERT( HAL_I2C_Master_Stop_Send(I2C_MASTER_SCB) == DRIVER_TRUE);
  printf("Read Register 0 = 0x%x\n",u8Data); /**<-- u8Data should be 0xaa*/
  
}

static void high_level_i2c_api_test(void)
{
  /** HAL_I2C_Master_Reg_Transmit / HAL_I2C_Master_Reg_Receive*/
  uint8_t u8RegData  = 0x55u;
  uint32_t u32Status = 0u;
  i2c_data_t i2c_pak = 
  {
    .u8SlaveAddress = DEVICE_ADDR,
    .u8RegAddress  = 0x00u,
    .pData = &u8RegData,
    .u8Length = 1u,
  };
  CY_ASSERT ( HAL_I2C_Master_Reg_Transmit(I2C_MASTER_SCB,&i2c_pak,&u32Status) == DRIVER_TRUE);
  printf("HAL_I2C_Master_Reg_Write Register 0 = 0x%x\n",u8RegData);
  
  /*Read it back device address 0x28 , register 0*/
  u8RegData = 0x00; /*clear data*/
  CY_ASSERT( HAL_I2C_Master_Reg_Receive(I2C_MASTER_SCB,&i2c_pak,&u32Status) == DRIVER_TRUE);
  printf("HAL_I2C_Master_Reg_Read Register 0 = 0x%x\n\n",u8RegData); /**<-- u8RegData should be 0x55*/
  
   Cy_SysLib_Delay(1300);
   
  /** HAL_I2C_Master_Transmit / HAL_I2C_Master_Receive*/
  uint8_t u8TxData[] = {0x00,0xaa}; /*Register/data*/
  i2c_data_t tsI2c_data = 
  {
    .u8SlaveAddress = DEVICE_ADDR,
    .pData = u8TxData,
    .u8Length = sizeof(u8TxData),
  };
  CY_ASSERT ( HAL_I2C_Master_Transmit(I2C_MASTER_SCB,&tsI2c_data,&u32Status) == DRIVER_TRUE);
  printf("HAL_I2C_Master_Transmit Register 0 = 0x%x\n",u8TxData[1u]);
  
  uint8_t u8RxData[4u];
  tsI2c_data.pData = u8RxData;
  tsI2c_data.u8Length = sizeof(u8RxData);
  /*Read it back device address 0x28 , register 0*/
  CY_ASSERT( HAL_I2C_Master_Receive(I2C_MASTER_SCB,&tsI2c_data,&u32Status) == DRIVER_TRUE);
  printf("HAL_I2C_Master_Receive  0x%x 0x%x 0x%x 0x%x\n",u8RxData[0],u8RxData[1],u8RxData[2],u8RxData[3]); /**<-- u8Data should be 0xaa*/
}

void testbed_i2c_master(void)
{
  __enable_irq();
  
  printf("\n\nI2C Master testbed\n");
  
  /** I2C Standard mode 100k bps*/
  gtsI2C_Master_Standard_Mode.u8PinSDA = I2C_PIN_SDA;
  gtsI2C_Master_Standard_Mode.u8PinSCL = I2C_PIN_SCL;
  uint8_t u8HalResult = HAL_I2C_Init( I2C_MASTER_SCB, &gtsI2C_Master_Standard_Mode);
  printf("I2C Master Init = %d\n",u8HalResult);
  HAL_I2C_Active( I2C_MASTER_SCB , FUNC_ENABLE);
  
  /*Low level I2C API*/
  low_level_i2c_api_test();
  
  Cy_SysLib_Delay(1300);
  
  /*High level I2C API*/ 
  high_level_i2c_api_test();
  
  for(;;);
}