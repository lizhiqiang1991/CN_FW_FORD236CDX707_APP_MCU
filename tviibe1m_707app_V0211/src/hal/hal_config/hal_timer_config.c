/***************************************************************************
*\file hal_timer_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_timer_def.h"

/*Timer Configuration*/
timer_config_t gts_1mSec_Timer_Continuous_Config = 
{
    .u32Period = 1000u,                 /*0~ 65535 uSec*/
    .u8RunType = TIMER_CONTINUOUS,      /* Continuous 、One Shot*/
    .u8InterruptEnable = FUNC_ENABLE,
    .u8NvicPriority = 3u,                //  it default is 0u ,SROM int use priority is 0 and 1 , so other INT can not occupy by Sean. modify by Adolf
    .u32Timeout = 0u,
};

timer_config_t gts_1mSec_Timer_OneShot_Config = 
{
    .u32Period = 1000u,                 /*0~ 65535 uSec*/
    .u8RunType = TIMER_ONESHOT,  /* Continuous 、One Shot*/
    .u8InterruptEnable = FUNC_ENABLE,
    .u8NvicPriority = 0u,
    .u32Timeout = 0u,
};

timer_config_t gts_10mSec_Timer_Continuous_Config =
{
    .u32Period = 10000u,                 /*0~ 65535 uSec*/
    .u8RunType = TIMER_CONTINUOUS,  /* Continuous 、One Shot*/
    .u8InterruptEnable = FUNC_ENABLE,
    .u8NvicPriority = 0u,
    .u32Timeout = 0u,
};

timer_config_t gts_2mSec_Timer_Continuous_Config = 
{
    .u32Period = 2000u,                 /*0~ 65535 uSec*/
    .u8RunType = TIMER_CONTINUOUS,      /* Continuous 、One Shot*/
    .u8InterruptEnable = FUNC_ENABLE,
    .u8NvicPriority = 0u,
    .u32Timeout = 0u,
};

timer_config_t gts_25mSec_Timer_Continuous_Config = 
{
    .u32Period = 25000u,                 /*0~ 65535 uSec*/
    .u8RunType = TIMER_CONTINUOUS,      /* Continuous 、One Shot*/
    .u8InterruptEnable = FUNC_ENABLE,
    .u8NvicPriority = 0u,
    .u32Timeout = 0u,
};

timer_config_t gts_50mSec_Timer_Continuous_Config = 
{
    .u32Period = 50000u,                 /*0~ 65535 uSec*/
    .u8RunType = TIMER_CONTINUOUS,      /* Continuous 、One Shot*/
    .u8InterruptEnable = FUNC_ENABLE,
    .u8NvicPriority = 0u,
    .u32Timeout = 0u,
};

/*** End of file ***/

  