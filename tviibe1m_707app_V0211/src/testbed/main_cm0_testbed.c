/*

File    : main_cm0_testbed.c
Purpose : Testbed application start

*/
#include "cy_project.h"
#include "cy_device_headers.h"
#include <stdio.h>
#include "hal_config.h"

#define GPIO_TESTBED               0
#define CLK_TESTBED                10
#define TIMER_TESTBED              20
#define PWM_TESTBED                30
#define PWM_PERIOD_TESTBED         40
#define I2C_MASTER_TESTBED         50
#define I2C_MEM_MASTER_TESTBED     51
#define I2C_SLAVE_TESTBED          60
#define I2C_LED_SLAVE_TESTBED      61
#define UART_POLLING_TESTBED       69
#define SP_TESTBED                 70
#define POWER_MODE_TESTBED         78
#define WDT_TESTBED                80
#define CHIPID_TESTBED             90
#define M4_TESTBED                 0xffff


extern void testbed_gpio  (void);
extern void testbed_clock (void);
extern void testbed_timer (void);
extern void testbed_pwm   (void);
extern void testbed_pwm_period(void);
extern void testbed_sp    (void); /**< check main stack usage*/ 
extern void testbed_wdt   (void);
extern void testbed_i2c_master (void);
extern void testbed_i2c_mem_master(void);
extern void testbed_i2c_slave (void);
extern void testbed_i2c_led_slave(void);
extern void testbed_uart_polling (void);
extern void testbed_power_mode(void);

#define CURRENT_TESTBED  ( GPIO_TESTBED )

void main(void) 
{
  /* Clock init*/
  HAL_Clock_Init(&gtsClockInit);

  #ifndef CY_SEMIHOSTING_DISABLED
    #if (UART_POLLING_TESTBED == CURRENT_TESTBED)
       /*Don't use uart semihosting */
    #else
      /*redirect printf to UART (SCB3) , P13.0 (RX) / P13.1 (TX)*/
      Cy_Semihosting_InitAll(CY_USB_SCB_TYPE,115200,NULL,false);
    #endif
  #endif
  
#if (GPIO_TESTBED == CURRENT_TESTBED)
  testbed_gpio();
#endif

#if (CLK_TESTBED == CURRENT_TESTBED)
  testbed_clock(); 
#endif

#if (TIMER_TESTBED == CURRENT_TESTBED)
  testbed_timer();
#endif

#if (PWM_TESTBED == CURRENT_TESTBED)
  testbed_pwm();
#endif

#if (PWM_PERIOD_TESTBED == CURRENT_TESTBED)
  testbed_pwm_period();
#endif  
  
#if (SP_TESTBED == CURRENT_TESTBED)
  testbed_sp();
#endif

#if (I2C_MASTER_TESTBED == CURRENT_TESTBED)
  testbed_i2c_master();
#endif

#if (I2C_MEM_MASTER_TESTBED == CURRENT_TESTBED)
  testbed_i2c_mem_master();
#endif
  
#if (I2C_SLAVE_TESTBED == CURRENT_TESTBED)
  testbed_i2c_slave();
#endif

#if (I2C_LED_SLAVE_TESTBED == CURRENT_TESTBED)
  testbed_i2c_led_slave();
#endif

#if (UART_POLLING_TESTBED == CURRENT_TESTBED)
  testbed_uart_polling();
#endif

#if (POWER_MODE_TESTBED == CURRENT_TESTBED)
  testbed_power_mode();
#endif
  
#if (WDT_TESTBED == CURRENT_TESTBED)
  //testbed_wdt(); /* has not been implemented yet.*/
#endif

#if (CHIPID_TESTBED == CURRENT_TESTBED)
  //testbed_chipID_sdl(); /* has not been implemented yet.*/
#endif
  
#if (M4_TESTBED == CURRENT_TESTBED)
  /* Enable CM4. CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
  Cy_SysEnableApplCore(CY_CORTEX_M4_APPL_ADDR); 
  for (;;)
  {
     Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
  }
#endif
  
  printf("NO TESTBED SELECTED\n");
  for (;;);
}

 
/*************************** End of file ****************************/
