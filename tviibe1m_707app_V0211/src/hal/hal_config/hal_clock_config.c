/***************************************************************************
*\file hal_clock_config.c
****************************************************************************/ 

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_clock_def.h"

/* 
        CYT2B6 Clocking System  
                
                               CLK_PATH0
       ----> PATH_MUX0 --> FLL ------------      --> ROOT_MUX0 --> CLK_HF0 (cm0/cm4/peri/flash ...)
  IMO  |                                  |      |
  ECO  |                                  |====> |
  EXT  |                       CLK_PATH1  |      | 
       ----> PATH_MUX1 --> PLL ------------      --> ROOT_MUX1 --> CLK_HF1 (event generator)

*/

clock_config_t gtsClockInit = 
{
    .u32EcoFreq   = 0u,
    .u8EcoEnable  = FUNC_DISABLE,
    .u8WcoEnable  = FUNC_DISABLE,
    .u8Ilo0Enable = FUNC_DISABLE,
    .u8Ilo1Enable = FUNC_DISABLE,
    .u32Timeout   = WAIT_FOR_STABILIZATION,
};

clock_config_t gtsClock_IMO_Init = 
{
    .u32EcoFreq   = 0u,
    .u8EcoEnable  = FUNC_ENABLE,
    .u8WcoEnable  = FUNC_DISABLE,
    .u8Ilo0Enable = FUNC_DISABLE,
    .u8Ilo1Enable = FUNC_DISABLE,
    .u32Timeout   = WAIT_FOR_STABILIZATION,
};

/*** End of file ***/

