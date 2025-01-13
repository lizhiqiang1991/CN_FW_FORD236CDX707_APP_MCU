/**
* @file I2CMasterApp.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-03-18
* 
* @copyright Copyright (c) 2021
* 
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "gPinDef.h"
#include "Ford_Config.h"
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_config.h"
#include "hal_gpio_def.h"
#include "hal_gpio.h"

#define I2SM_RBUFFER 32U

/* TCON Magic Code */
#define TCON_MAGIC_CODE             0x55U 

#define TCON_ADDRESS_ID             0x52U
#define SOURCE_IC_ADDRESS_58H       0x58U
#define SOURCE_IC_ADDRESS_59H       0x59U
#define SOURCE_IC_ADDRESS_5AH       0x5AU
#define SOURCE_IC_ADDRESS_5BH       0x5BU
#define TCON_CHIPID_ADDR            0x00U   
#define TCON_GLOBAL_DUTY_ADDR_W     0x0AU   //0x02 -> 0x0A
#define TCON_GLOBAL_DUTY_ADDR_R     0x0BU   //0x03 -> 0x0B
#define TCON_FAIL_FLAG_ADDR         0x10U
#define TCON_LOCAL_DIM_ONOFF        0x04h

#define TCON_LED_STATUS1            0x60U  /* Left side led  */
#define TCON_LED_STATUS2            0x61U  /* right side led */
#define TCON_FAIL_FLAG              0x10U
#define TCON_VERSION                0x0EU

#define TCON_LED_FAIL_MASK          0x70U   // 0x70U -> 0x30U  ->  0x10
#define TCON_LED_STATUS_MASK        0x02U

#define TCON_CHIPID_LENGTH           5U   
#define TCON_FAIL_FLAG_LENGTH        2U
#define TCON_LED_STATUS_LENGTH       24U
#define TCON_GLOBALDUTY_LENGTH       2U      
#define TCON_CHECKSUM_LENGTH         1U
#define TCON_MAGICCODE_LENGTH        1U    
#define TCON_VERSION_LENGTH          8U

#define TFTBIAS_ADDRESS_ID          0x21
#define TFT_REV_ID_MASK             0x07

typedef enum
{
	eI2C_TCON_Magic_Wr = 0,
	eI2C_Wait2ms_T1,
	eI2C_Wait2ms_T2,
    eI2C_Wait2ms_T3,
	eI2C_TCON_Reg_Read
}I2C_TconStrategyState_E;

void I2CM_Initialze(void);
void I2CM_DeInitialze(void);
void I2CM_TEST_API(void);
uint8_t I2CM_Write(uint8_t Device_ID, uint8_t Reg_Addr, uint8_t * p_WData, uint8_t u8Length);
uint8_t I2CM_Read(uint8_t Device_ID, uint8_t Reg_Addr, uint8_t * p_WData, uint8_t u8Length);


#if BACKDOOR_ICDIAG_OPEN
void I2CM_Diag(uint8_t u8DeviceAddr, uint8_t u8DataAddr, uint8_t *u8TxData, uint8_t u8TxLen, uint8_t *u8RxData, uint8_t u8RxLen);
#endif