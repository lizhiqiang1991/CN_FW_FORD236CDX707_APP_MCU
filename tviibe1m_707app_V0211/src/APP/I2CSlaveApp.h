/**
* @file I2CSlaveApp.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-02-02
* 
* @copyright Copyright (c) 2021
* 
*/
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_config.h"
#include "hal_gpio_def.h"
#include "hal_gpio.h"


#define I2CS_WBUFFER_SIZE 256U
#define I2CS_RBUFFER_SIZE 67U  //32U -> 67U

void I2CS_Initialze(void);
void I2CS_DeInitialze(void);
