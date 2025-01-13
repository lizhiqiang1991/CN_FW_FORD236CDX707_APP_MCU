/***************************************************************************
*\file hal_pm_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_pm_def.h"

pm_config_t gtsSleepMode = 
{
   .u8Mode = PM_LOW_POWER_LV2
};

pm_config_t gtsDeepSleepMode = 
{
   .u8Mode = PM_LOW_POWER_LV3
};

pm_config_t gtsHIBMode_Pin0_low_wakeup = 
{
   .u32HibernateWakeupSource = SYSPM_HIBPIN0_LOW,
   .u8Mode = PM_LOW_POWER_LV4
};

pm_config_t gtsHIBMode_Pin0_Hi_wakeup = 
{
   .u32HibernateWakeupSource = CY_SYSPM_HIBPIN0_HIGH,
   .u8Mode = PM_LOW_POWER_LV4
};

pm_config_t gtsHIBMode_Pin1_low_wakeup = 
{
   .u32HibernateWakeupSource = SYSPM_HIBPIN1_LOW,
   .u8Mode = PM_LOW_POWER_LV4
};

pm_config_t gtsHIBMode_Pin1_Hi_wakeup = 
{
   .u32HibernateWakeupSource = CY_SYSPM_HIBPIN1_HIGH,
   .u8Mode = PM_LOW_POWER_LV4
};

pm_config_t gts_OVD_BOD_Enable = 
{
  .u8Mode               = PM_LOW_POWER_LV1,
  .u8OverVoltageVDDD    = OVD_VDDD_5_0V,
  .u8OverVoltageVDDA    = OVD_VDDA_5_0V,
  .u8OverVoltageAction  = OVD_ACTION_RESET,
  .u8OverVoltageEnable  = FUNC_ENABLE,
  .u8BODThresholdVDDD   = BOD_VDDD_2_7V,
  .u8BODThresholdVDDA   = BOD_VDDA_2_7V, 
  .u8BODAction          = BOD_ACTION_RESET,
  .u8BODEnable          = FUNC_ENABLE,
};

/*** End of file ***/

  