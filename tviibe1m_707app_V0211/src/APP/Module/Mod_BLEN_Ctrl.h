/**
* @file Mod_BLEN_Ctrl.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-04-08
* 
* @copyright Copyright (c) 2021
* 
*/
#ifndef MOD_BLEN_CTRL_H_
#define MOD_BLEN_CTRL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include "gPinDef.h"

uint8_t MBLC_Display_Ctrl(uint8_t ucBLEN,Global_PowerState_E ePowerState,Global_LockState_E eLockState);

#endif
