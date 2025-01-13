/***************************************************************************
*\file hal_pm.h 
****************************************************************************/

#ifndef HAL_PM_H
#define HAL_PM_H

uint8_t HAL_PM_Init           (pm_config_t* tsConfig);
uint8_t HAL_PM_LowPower_Enter (pm_config_t* tsConfig);

#endif

  