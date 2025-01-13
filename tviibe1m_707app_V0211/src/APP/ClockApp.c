/**
* @file ClockApp.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-02-02
* 
* @copyright Copyright (c) 2021
* 
*/
/* Include standard */
//#include <stdio.h>
//#include <stdint.h>

/* include HAL */
#include "ClockApp.h"


void Clock_Initialize(void)
{
    HAL_Clock_Init(&gtsClockInit);
}