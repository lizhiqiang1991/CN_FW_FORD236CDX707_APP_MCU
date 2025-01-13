/**
* @file WDTApp.h
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
#include "hal_config.h"

#define WDT_COUNT_VALUE 32767ul /* 1000ms/30.518us=32767, Unit : 1/32768Hz=30.518us, Upper Limit: 1sec (override) */

void WDT_Initialize(void);
void WDTApp_ClearWDT (void);