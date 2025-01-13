/***************************************************************************
*\file hal_pm.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_pm_def.h"
#include "hal_pm.h"

static void CYHAL_OVD_Active(pm_config_t* tsConfig)
{
  if (FUNC_ENABLE == tsConfig->u8OverVoltageEnable)
  {
    Cy_SysPm_OvdVdddSelect((cy_en_syspm_ovd_vddd_sel_t)tsConfig->u8OverVoltageVDDD);
    Cy_SysPm_OvdVddaSelect((cy_en_syspm_ovd_vdda_sel_t)tsConfig->u8OverVoltageVDDA);
    Cy_Syspm_OvdActionSelect((cy_en_syspm_ovd_action_select_t)tsConfig->u8OverVoltageAction);
    Cy_SysPm_OvdEnable(CY_SYSPM_OVD_ON_VDDD);
    Cy_SysPm_OvdEnable(CY_SYSPM_OVD_ON_VDDA);
  }
  else
  {
    Cy_SysPm_OvdDisable(CY_SYSPM_OVD_ON_VDDD);
    Cy_SysPm_OvdDisable(CY_SYSPM_OVD_ON_VDDA);
  }
}

static void CYHAL_BOD_Active(pm_config_t* tsConfig)
{
  if (FUNC_ENABLE == tsConfig->u8BODEnable)
  {
    Cy_SysPm_BodVdddSelect((cy_en_syspm_bod_vddd_sel_t)tsConfig->u8BODThresholdVDDD );
    Cy_SysPm_BodVddaSelect((cy_en_syspm_bod_vdda_sel_t)tsConfig->u8BODThresholdVDDA);
    Cy_Syspm_BodActionSelect((cy_en_syspm_bod_action_select_t)tsConfig->u8BODAction);
    Cy_SysPm_BodEnable(CY_SYSPM_BOD_ON_VDDD);
    Cy_SysPm_BodEnable(CY_SYSPM_BOD_ON_VDDA);
  }
  else
  {
    Cy_SysPm_BodDisable(CY_SYSPM_BOD_ON_VDDA);
    Cy_SysPm_BodDisable(CY_SYSPM_BOD_ON_VDDD);
  }
}

uint8_t HAL_PM_Init(pm_config_t* tsConfig)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  if (NULL != tsConfig)
  {
    /* OVD enable / disable*/
    CYHAL_OVD_Active(tsConfig);
    /* BOD enable / disable*/
    CYHAL_BOD_Active(tsConfig);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_PM_LowPower_Enter (pm_config_t* tsConfig)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  if (NULL != tsConfig)
  {
    switch (tsConfig->u8Mode)
    {
    case PM_LOW_POWER_LV1: /*Active*/
      /*Do nothing*/
      u8HalResult = DRIVER_TRUE;
      break;
      
    case PM_LOW_POWER_LV2: /*Sleep*/
      Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
      u8HalResult = DRIVER_TRUE;
      break;
      
    case PM_LOW_POWER_LV3: /*DeepSleep*/
      Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
      u8HalResult = DRIVER_TRUE;
      break;
      
    case PM_LOW_POWER_LV4: /*Hibernate*/
      Cy_SysPm_SetHibWakeupSource((cy_en_syspm_hib_wakeup_source_t)tsConfig->u32HibernateWakeupSource);
      Cy_SysPm_Hibernate();
      /* Won't reach here */
      u8HalResult = DRIVER_TRUE;
      break;
    default:
      u8HalResult = DRIVER_REGISTER_ERROR;
      break;
    }
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  
  return u8HalResult;
}

/*** End of file ***/

  