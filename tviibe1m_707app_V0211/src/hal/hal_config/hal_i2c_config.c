/***************************************************************************
*\file hal_i2c_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_i2c_def.h"
#include "hal_gpio_def.h"


#define I2C_BUS_MASTER_TIMEOUT   (1U)  //unit 1ms

/** I2C Master mode config*/
i2c_config_t gtsI2C_Master_Standard_Mode = /**< 100k bps*/
{
  .u8PinSCL          = IO_NC,
  .u8PinSDA          = IO_NC,
  .u8RoleMode        = I2C_MASTER_MODE,
  .u8SpeedMode       = I2C_STANDARD_MODE,
  .u32ClockSpeed     = I2C_STANDARD_MODE_INCLK_FREQ,
  .u32Timeout        = I2C_BUS_MASTER_TIMEOUT,
};

i2c_config_t gtsI2C_Master_Fast_Mode = /**< 400k bps*/
{
  .u8PinSCL          = IO_NC,  
  .u8PinSDA          = IO_NC,
  .u8RoleMode        = I2C_MASTER_MODE,
  .u8SpeedMode       = I2C_FAST_MODE,
  .u32ClockSpeed     = I2C_FAST_MODE_INCLK_FREQ,
  .u32Timeout        = I2C_BUS_MASTER_TIMEOUT,
};

i2c_config_t gtsI2C_Master_FastPlus_Mode = /**< 1000k bps*/
{
  .u8PinSCL          = IO_NC,  
  .u8PinSDA          = IO_NC,
  .u8RoleMode        = I2C_MASTER_MODE,
  .u8SpeedMode       = I2C_FAST_PLUS_MODE,
  .u32ClockSpeed     = I2C_FAST_PLUS_MODE_INCLK_FREQ,
  .u32Timeout        = I2C_BUS_MASTER_TIMEOUT,
};


/** I2C Slave mode config*/
i2c_config_t gtsI2C_Slave_Standard_Mode = /**< 100k bps*/
{
  .u8PinSCL          = IO_NC,  
  .u8PinSDA          = IO_NC,
  .u8RoleMode        = I2C_SLAVE_MODE,
  .u8SpeedMode       = I2C_STANDARD_MODE,
  .u32ClockSpeed     = I2C_STANDARD_MODE_INCLK_FREQ,
};

i2c_config_t gtsI2C_Slave_Fast_Mode = /**< 400k bps*/
{
  .u8PinSCL          = IO_NC,  
  .u8PinSDA          = IO_NC,
  .u8RoleMode        = I2C_SLAVE_MODE,
  .u8SpeedMode       = I2C_FAST_MODE,
  .u32ClockSpeed     = I2C_FAST_MODE_INCLK_FREQ,
};

i2c_config_t gtsI2C_Slave_FastPlus_Mode = /**< 1000k bps*/
{
  .u8PinSCL          = IO_NC,  
  .u8PinSDA          = IO_NC,
  .u8RoleMode        = I2C_SLAVE_MODE,
  .u8SpeedMode       = I2C_FAST_PLUS_MODE,
  .u32ClockSpeed     = I2C_FAST_PLUS_MODE_INCLK_FREQ,
};

/*** End of file ***/

  