#ifndef HAL_CFD_H
#define HAL_CFD_H

#define HAL_GPIO_ENABLED
#define HAL_CLOCK_ENABLED
#define HAL_SP_ENABLED
#define HAL_TIMER_ENABLED
#define HAL_PWM_ENABLED
#define HAL_I2C_ENABLED
#define HAL_UART_ENABLED
#define HAL_ADC_ENABLED
#define HAL_PWR_MODE_ENABLED
#define HAL_FMC_ENABLED

/* Not implemented yet
  #define HAL_SPI_ENABLED
  #define HAL_WDT_ENABLED
 
  #define HAL_EEPROM_ENABLED
  #define HAL_FMC_ENABLED
*/
 
#include "hal_common.h"

#ifdef HAL_GPIO_ENABLED
  #include "hal_gpio_def.h"
  #include "hal_gpio.h"
#endif

#ifdef HAL_CLOCK_ENABLED
  #include "hal_clock_def.h"
  #include "hal_clock.h"
#endif

#ifdef HAL_TIMER_ENABLED
  #include "hal_timer_def.h"
  #include "hal_timer.h"
#endif

#ifdef HAL_SP_ENABLED
  #include "hal_sp_def.h"
  #include "hal_sp.h"
#endif

#ifdef HAL_SYS_ENABLED
  #include "hal_sys.h"
#endif

#ifdef HAL_WDT_ENABLED
  #include "hal_wdt_def.h"
  #include "hal_wdt.h"
#endif

#ifdef HAL_ADC_ENABLED
  #include "hal_adc_def.h"
  #include "hal_adc.h"
#endif

#ifdef HAL_I2C_ENABLED
  #include "hal_i2c_def.h"
  #include "hal_i2c.h"
#endif  

#ifdef HAL_UART_ENABLED
  #include "hal_uart_def.h"
  #include "hal_uart.h"
#endif  

#ifdef HAL_PWM_ENABLED
  #include "hal_pwm_def.h"
  #include "hal_pwm.h"
#endif

#ifdef HAL_EEPROM_ENABLED
  #include "hal_eeprom.h"
#endif

#ifdef HAL_FMC_ENABLED
  #include "hal_fmc_def.h"
  #include "hal_fmc.h"
#endif

#ifdef HAL_PWR_MODE_ENABLED
  #include "hal_pm_def.h"
  #include "hal_pm.h"
#endif

#endif

