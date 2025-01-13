/***************************************************************************
*\file hal_pwm_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_pwm_def.h"
#include "hal_gpio_def.h"

/*PWM Configuration*/
pwm_config_t gts_2K_hz_Config = 
{
  /*
  Peri clock = 80Mhz
  u8PreScaler = 4 , 80Mhz/4 = 20Mhz
  Period = 10,000 , 20Mhz / 10,000 = 2Khz 
  Compare = 5,000  , 50% PWM duty (5,000/10,000)
  PWM OUTPUT => 2Khz , 50% duty 
  */          
  .u8PreScaler        = PWM_PRESCALER_DIVBY_4,  /**< 1/2/4/8/16/32/64/128 */
  .u8Pin              = IO_NC,                  /**< pwm output pin*/
  .u8PinComplementary = IO_NC,                  /**< An optional, additional inverted output pin.*/
  .u16Period          = 10000U,                 /**< 0 ~ 65535*/
  .u16Duty            = 5000U,                  /**< 0 ~ 65535 Compare */
  .u8Mode             = PWM_LEFT_ALIGN,         /**< PWM alignment */
  .u8RunType          = PWM_CONTINUOUS,         /**< Continuous or One Shot*/ 
  .u8Invert           = FUNC_DISABLE,            /**< An option for the user to invert the PWM output*/
  .u8InterruptEnable  = FUNC_DISABLE,            /**< Enable/Disable Interrupt*/
  .u8NvicPriority     = 0U,                     /**< ISR priority*/
  .u32Timeout         = 0U,                     /**< PWM Config timeout */
};

/*** End of file ***/

  