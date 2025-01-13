/***************************************************************************
*\file hal_gpio.c
****************************************************************************/ 

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_gpio_def.h"

#define OUT_VALUE_SET(value,position)           (value << position)
#define DRIVE_MODE_SET(value,position)          (value << (position * 4))
#define VTRIP_SET(value,position)               (value << position)
#define DRIVE_SET(value,position)               (value << (16+ position * 2))
#define HSIOM0_SET(value,position)              (value << (position * 8))
#define HSIOM1_SET(value,position)              (value << ((position - 4) * 8))
/***************************************************************************
*\example 
****************************************************************************/
/***************** Pin Output Configuration ********************************/
gpio_pin_config_t gtdPinOutputConfig_Test = 
{
    .u8OutValue =  PIN_HIGH,                     /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF,    /**< Drive mode (OUTPUT)*/
    .u8HSIOM = HSIOM_SEL_GPIO,                  /**< HSIOM selection gpio default = 0*/
    .u8Vtrip = CY_GPIO_VTRIP_CMOS,              /**< Input buffer voltage trip type */
    .u8SlewRate = CY_GPIO_SLEW_FAST,            /**< Output buffer slew rate */
    .u8DriveStength = CY_GPIO_DRIVE_FULL,       /**< Drive strength */
};

gpio_pin_config_t gtdPinOutputConfig_Test2 = 
{
    .u8OutValue =  PIN_HIGH,                     /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_PULLUP,    /**< Drive mode (OUTPUT)*/
    .u8HSIOM = HSIOM_SEL_GPIO,                  /**< HSIOM selection gpio default = 0*/
    .u8Vtrip = CY_GPIO_VTRIP_CMOS,              /**< Input buffer voltage trip type */
    .u8SlewRate = CY_GPIO_SLEW_FAST,            /**< Output buffer slew rate */
    .u8DriveStength = CY_GPIO_DRIVE_FULL,       /**< Drive strength */
};

gpio_pin_config_t gtdPinOutputConfig = 
{
    .u8OutValue =  PIN_LOW,                     /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF,    /**< Drive mode (OUTPUT)*/
    .u8HSIOM = HSIOM_SEL_GPIO,                  /**< HSIOM selection gpio default = 0*/
    .u8Vtrip = CY_GPIO_VTRIP_CMOS,              /**< Input buffer voltage trip type */
    .u8SlewRate = CY_GPIO_SLEW_FAST,            /**< Output buffer slew rate */
    .u8DriveStength = CY_GPIO_DRIVE_FULL,       /**< Drive strength */
};
gpio_pin_config_t gtdPinOutputConfig_OD_GPIO = 
{
    .u8OutValue =  PIN_HIGH,                     /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_OD_DRIVESLOW_IN_OFF,    /**< Drive mode (OUTPUT)*/
    .u8HSIOM = HSIOM_SEL_GPIO,                  /**< HSIOM selection gpio default = 0*/
    .u8Vtrip = CY_GPIO_VTRIP_CMOS,              /**< Input buffer voltage trip type */
    .u8SlewRate = CY_GPIO_SLEW_FAST,            /**< Output buffer slew rate */
    .u8DriveStength = CY_GPIO_DRIVE_FULL,       /**< Drive strength */
};
gpio_pin_config_t gtdPinOutputConfig_PWM_GPIO = 
{
    .u8OutValue =  PIN_HIGH,                     /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF,    /**< Drive mode (OUTPUT)*/
    .u8HSIOM = HSIOM_SEL_GPIO,                  /**< HSIOM selection gpio default = 0*/
    .u8Vtrip = CY_GPIO_VTRIP_CMOS,              /**< Input buffer voltage trip type */
    .u8SlewRate = CY_GPIO_SLEW_FAST,            /**< Output buffer slew rate */
    .u8DriveStength = CY_GPIO_DRIVE_FULL,       /**< Drive strength */
};

  gpio_pin_config_t tsGpioCfg_GPIO_PWM = 
  {
    .u8OutValue =  PIN_HIGH,                          /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF,    /**< Drive mode (OUTPUT)*/
    .u8HSIOM = HSIOM_SEL_ACT_0,                 /**< HSIOM selection tcpwm[0].line[x]*/
    .u8Vtrip = 0U,                              /**< Input buffer voltage trip type */
    .u8SlewRate = 0U,                           /**< Output buffer slew rate */
    .u8DriveStength = 0U,                       /**< Drive strength */
  };
/****************** Pin Input Configuration ********************************/
gpio_pin_config_t gtdPinInputConfig = 
{
    .u8OutValue =  PIN_LOW,                     /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_HIGHZ,            /**< Drive mode (INPUT)*/
    .u8HSIOM = HSIOM_SEL_GPIO,                  /**< HSIOM selection gpio default = 0*/
    .u8Vtrip = CY_GPIO_VTRIP_CMOS,              /**< Input buffer voltage trip type */
    .u8SlewRate = CY_GPIO_SLEW_FAST,            /**< Output buffer slew rate */
    .u8DriveStength = CY_GPIO_DRIVE_FULL,       /**< Drive strength */
};
/**************** Port Output Configuration ********************************/
gpio_port_config_t gtdPortOutputConfig = 
{
/**< Initial output data for the IO pins in the port */
    .u32OutValue = (OUT_VALUE_SET(PIN_LOW,7U) |\
                    OUT_VALUE_SET(PIN_LOW,6U) |\
                    OUT_VALUE_SET(PIN_LOW,5U) |\
                    OUT_VALUE_SET(PIN_LOW,4U) |\
                    OUT_VALUE_SET(PIN_LOW,3U) |\
                    OUT_VALUE_SET(PIN_LOW,2U) |\
                    OUT_VALUE_SET(PIN_LOW,1U) |\
                    OUT_VALUE_SET(PIN_LOW,0U)),

/**< Port pin drive modes and input buffer enable configuration */
    .u32DriveMode = (DRIVE_MODE_SET(CY_GPIO_DM_STRONG_IN_OFF,7U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_STRONG_IN_OFF,6U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_STRONG_IN_OFF,5U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_STRONG_IN_OFF,4U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_STRONG_IN_OFF,3U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_STRONG_IN_OFF,2U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_STRONG_IN_OFF,1U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_STRONG_IN_OFF,0U)),

/**< Port pin input buffer configuration */
    .u32CfgIn = (VTRIP_SET(CY_GPIO_VTRIP_CMOS,7U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,6U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,5U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,4U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,3U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,2U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,1U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,0U)),

/**< Port pin output buffer configuration */
    .u32CfgOut = (DRIVE_SET(CY_GPIO_DRIVE_FULL,7U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,6U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,5U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,4U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,3U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,2U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,1U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,0U)),

/**< HSIOM selection for port pins 0,1,2,3 */
    .u32HSIOM0 = (HSIOM0_SET(HSIOM_SEL_GPIO,3U) |\
                  HSIOM0_SET(HSIOM_SEL_GPIO,2U) |\
                  HSIOM0_SET(HSIOM_SEL_GPIO,1U) |\
                  HSIOM0_SET(HSIOM_SEL_GPIO,0U)),

/**< HSIOM selection for port pins 4,5,6,7 */
    .u32HSIOM1 = (HSIOM1_SET(HSIOM_SEL_GPIO,7U) |\
                  HSIOM1_SET(HSIOM_SEL_GPIO,6U) |\
                  HSIOM1_SET(HSIOM_SEL_GPIO,5U) |\
                  HSIOM1_SET(HSIOM_SEL_GPIO,4U)),
};

/**************** Port Input Configuration *********************************/
gpio_port_config_t gtdPortInputConfig = 
{
/**< Initial output data for the IO pins in the port */
    .u32OutValue = (OUT_VALUE_SET(PIN_LOW,7U) |\
                    OUT_VALUE_SET(PIN_LOW,6U) |\
                    OUT_VALUE_SET(PIN_LOW,5U) |\
                    OUT_VALUE_SET(PIN_LOW,4U) |\
                    OUT_VALUE_SET(PIN_LOW,3U) |\
                    OUT_VALUE_SET(PIN_LOW,2U) |\
                    OUT_VALUE_SET(PIN_LOW,1U) |\
                    OUT_VALUE_SET(PIN_LOW,0U)),

/**< Port pin drive modes and input buffer enable configuration */
    .u32DriveMode = (DRIVE_MODE_SET(CY_GPIO_DM_HIGHZ,7U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_HIGHZ,6U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_HIGHZ,5U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_HIGHZ,4U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_HIGHZ,3U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_HIGHZ,2U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_HIGHZ,1U) |\
                     DRIVE_MODE_SET(CY_GPIO_DM_HIGHZ,0U)),

/**< Port pin input buffer configuration */
    .u32CfgIn = (VTRIP_SET(CY_GPIO_VTRIP_CMOS,7U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,6U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,5U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,4U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,3U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,2U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,1U) |
                 VTRIP_SET(CY_GPIO_VTRIP_CMOS,0U)),

/**< Port pin output buffer configuration */
    .u32CfgOut = (DRIVE_SET(CY_GPIO_DRIVE_FULL,7U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,6U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,5U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,4U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,3U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,2U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,1U) |\
                  DRIVE_SET(CY_GPIO_DRIVE_FULL,0U)),

/**< HSIOM selection for port pins 0,1,2,3 */
    .u32HSIOM0 = (HSIOM0_SET(HSIOM_SEL_GPIO,3U) |\
                  HSIOM0_SET(HSIOM_SEL_GPIO,2U) |\
                  HSIOM0_SET(HSIOM_SEL_GPIO,1U) |\
                  HSIOM0_SET(HSIOM_SEL_GPIO,0U)),

/**< HSIOM selection for port pins 4,5,6,7 */
    .u32HSIOM1 = (HSIOM1_SET(HSIOM_SEL_GPIO,7U) |\
                  HSIOM1_SET(HSIOM_SEL_GPIO,6U) |\
                  HSIOM1_SET(HSIOM_SEL_GPIO,5U) |\
                  HSIOM1_SET(HSIOM_SEL_GPIO,4U)),
};
