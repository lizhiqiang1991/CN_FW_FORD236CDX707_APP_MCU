/**
* @file WDTApp.c
* @author Derek Wen
* @brief 
* @version 0.1
* @date 2021-06-28
* 
* @copyright Copyright (c) 2021
* 
*/
/* Include standard */
#include <stdio.h>
#include <stdint.h>

/* include HAL */
#include "WDTApp.h"



void WDT_Initialize(void)
{
    /* Clear the PORVDDD Reset reason flag */
    /* This is necessary because PORVDDD flag blocks the WDT flag to rise */
    Cy_SysReset_ClearAllResetReasons();
    
    /*-----------------------*/
    /* Configuration for WDT */
    /*-----------------------*/
    Cy_WDT_Init();                      /* Upper Limit: 1sec and reset */
    Cy_WDT_Unlock();
    Cy_WDT_SetUpperLimit(WDT_COUNT_VALUE);      /* Unit : 1/32768Hz=30.518us, Upper Limit: 1sec (override) */
    Cy_WDT_SetDebugRun(CY_WDT_ENABLE);  /* This is necessary when using debugger */
    Cy_WDT_Lock();
    Cy_WDT_Enable();
}

void WDTApp_ClearWDT (void)
{
    /* When testing WDT reset, this code needs to be commented out. */
    Cy_WDT_ClearWatchdog();
}