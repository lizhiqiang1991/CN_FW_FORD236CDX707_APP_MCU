/***************************************************************************
*\file hal_systick_config.c 
****************************************************************************/
#if 0
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_systick_def.h"

systick_config_t gtdSystickConfig = 
{
     .u32ReloadCounter = 10000u,
     .u32Counter = 10000u,
     .u8ClockSource =SYSTICK_CLOCK_SOURCE_CLK_CPU,
     .u8InterruptEnable = FUNC_DISABLE,
     .u8NvicPriority = 3u,
};
#endif
/*** End of file ***/

  