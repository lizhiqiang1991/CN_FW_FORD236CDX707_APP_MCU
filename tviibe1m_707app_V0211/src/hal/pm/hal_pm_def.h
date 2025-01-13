/***************************************************************************
*\file hal_pm_def.h 
****************************************************************************/

#ifndef HAL_PM_DEF_H
#define HAL_PM_DEF_H

typedef struct
{
  uint8_t  u8Mode;
  uint8_t  u8IOFreeze;
  uint32_t u32HibernateWakeupSource;
  uint8_t u8OverVoltageVDDD;
  uint8_t u8OverVoltageVDDA; 
  uint8_t u8OverVoltageAction;
  uint8_t u8OverVoltageEnable;
  uint8_t u8BODThresholdVDDD;
  uint8_t u8BODThresholdVDDA; 
  uint8_t u8BODAction;
  uint8_t u8BODEnable;
}pm_config_t;

/**
 * Low power modes
 */
#define PM_LOW_POWER_LV1   (0U)  /*Active*/
#define PM_LOW_POWER_LV2   (1U)  /*Sleep*/
#define PM_LOW_POWER_LV3   (2U)  /*DeepSleep*/
#define PM_LOW_POWER_LV4   (3U)  /*Hibernate*/

/**
 *   Hibernate mode wakeup source
 */
#define SYSPM_HIBALARM      CY_SYSPM_HIBALARM       /** Configure the RTC alarm */
#define SYSPM_HIBWDT        CY_SYSPM_HIBWDT         /** Configure the WDT interrupt */
#define SYSPM_HIBPIN0_LOW   CY_SYSPM_HIBPIN0_LOW    /** Configure a low level for the first wakeup-pin */
#define SYSPM_HIBPIN0_HIGH  CY_SYSPM_HIBPIN0_HIGH   /** Configure a high level for the first wakeup-pin */
#define SYSPM_HIBPIN1_LOW   CY_SYSPM_HIBPIN1_LOW    /** Configure a low level for the second wakeup-pin */
#define SYSPM_HIBPIN1_HIGH  CY_SYSPM_HIBPIN1_HIGH   /** Configure a high level for the second wakeup-pin */

/**
 * BOD action select configuration.
 */
#define BOD_ACTION_NOTHING  CY_SYSPM_BOD_ACTION_NOTHING      
#define BOD_ACTION_FAULT    CY_SYSPM_BOD_ACTION_FAULT          
#define BOD_ACTION_RESET    CY_SYSPM_BOD_ACTION_RESET

/**
 * OVD action select configuration.
 */   
#define OVD_ACTION_NOTHING  CY_SYSPM_OVD_ACTION_NOTHING      
#define OVD_ACTION_FAULT    CY_SYSPM_OVD_ACTION_FAULT          
#define OVD_ACTION_RESET    CY_SYSPM_OVD_ACTION_RESET

/**
* Selects the voltage threshold for BOD on VDDD
*/
#define BOD_VDDD_2_7V       CY_SYSPM_BOD_VDDD_2_7V
#define BOD_VDDD_3_0V       CY_SYSPM_BOD_VDDD_3_0V
/**
* Selects the voltage threshold for BOD on VDDA
*/
#define BOD_VDDA_2_7V       CY_SYSPM_BOD_VDDA_2_7V
#define BOD_VDDA_3_0V       CY_SYSPM_BOD_VDDA_3_0V
/**
* Selects the voltage threshold for OVD on VDDD
*/
#define OVD_VDDD_5_5V       CY_SYSPM_OVD_VDDD_5_5V
#define OVD_VDDD_5_0V       CY_SYSPM_OVD_VDDD_5_0V
/**
* Selects the voltage threshold for OVD on VDDA
*/
#define OVD_VDDA_5_5V       CY_SYSPM_OVD_VDDA_5_5V
#define OVD_VDDA_5_0V       CY_SYSPM_OVD_VDDA_5_0V

extern pm_config_t gtsSleepMode;
extern pm_config_t gtsDeepSleepMode; 
extern pm_config_t gtsHIBMode_Pin0_low_wakeup;
extern pm_config_t gtsHIBMode_Pin0_Hi_wakeup;
extern pm_config_t gtsHIBMode_Pin1_low_wakeup;
extern pm_config_t gtsHIBMode_Pin1_Hi_wakeup;
extern pm_config_t gts_OVD_BOD_Enable;
 
#endif

  