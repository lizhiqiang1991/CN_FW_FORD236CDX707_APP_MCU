/*

File    : testbed_power_mode.c
Purpose : power mode testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

#define SLEEP_POWER_MODE          0x10u
#define DEEP_SLEEP_POWER_MODE     0x20u
#define HIBERNATE_POWER_MODE      0x30u

#define CURRENT_POWER_MODE ( DEEP_SLEEP_POWER_MODE )

#define USER_BUTTON        KIT_BUTTON_PIN    /* P7_0 SW1 CYTVII-B-E-1M-SK*/
#define USER_LED_1         KIT_LED_1_BLUE 
#define USER_LED_2         KIT_LED_4_BLUE 

/* Setup GPIO for BUTTON interrupt */
const cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = ioss_interrupts_gpio_7_IRQn, /*GPIO Port Interrupt #7 */
    .intIdx     = CPUIntIdx7_IRQn,
    .isEnabled  = true,
};

const cy_stc_gpio_pin_config_t user_button_port_pin_cfg = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom     = HSIOM_SEL_GPIO,
    .intEdge   = CY_GPIO_INTR_FALLING,
    .intMask   = 1ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

static void ButtonIntHandler(void)
{
  uint32_t intStatus;
  
  /* If falling edge detected */
  intStatus = Cy_GPIO_GetInterruptStatusMasked(GPIO_PRT7, 0U);
  if (intStatus != 0ul)
  {
    HAL_GPIO_PinToggle(USER_LED_2);
    Cy_GPIO_ClearInterrupt(GPIO_PRT7, 0U);
  }
}

#if (SLEEP_POWER_MODE == CURRENT_POWER_MODE)
static void SleepModeDemo(void)
{
  printf("Enter Sleep mode\nPress USER Button to wakeup ...\n");
  HAL_GPIO_PinWrite(USER_LED_1,PIN_HIGH);
  Cy_SysLib_Delay(300u); /*Delay while for uart printf*/
  
  HAL_PM_LowPower_Enter(&gtsSleepMode);
  
  HAL_GPIO_PinWrite(USER_LED_1,PIN_LOW);
  printf("Wakeup form Sleep mode\n\n");
  Cy_SysLib_Delay(2000u); /*Delay 2sec for uart & LED*/
}
#endif

#if (DEEP_SLEEP_POWER_MODE == CURRENT_POWER_MODE)
static void DeepSleepModeDemo(void)
{
  printf("Enter DeepSleep mode\nPress USER Button to wakeup ...\n");
  HAL_GPIO_PinWrite(USER_LED_1,PIN_HIGH);
  Cy_SysLib_Delay(300u); /*Delay while for uart printf*/
  
  HAL_PM_LowPower_Enter(&gtsDeepSleepMode);
  
  HAL_GPIO_PinWrite(USER_LED_1,PIN_LOW);
  printf("Wakeup form DeepSleep mode\n\n");
  Cy_SysLib_Delay(2000u); /*Delay 2sec for uart & LED*/
}
#endif

#if (HIBERNATE_POWER_MODE == CURRENT_POWER_MODE)
static void HibernateModeDemo(void)
{
  printf("Enter Hibernate mode\n");
  HAL_GPIO_PinWrite(USER_LED_1,PIN_HIGH);
  Cy_SysLib_Delay(300u); /*Delay while for uart printf*/
  
  HAL_PM_LowPower_Enter(&gtsHIBMode_Pin1_low_wakeup);
  /* Won't reach here */
  printf("Hibernate mode won't reach here\n");
}
#endif

void testbed_power_mode(void)
{
  __enable_irq();
  
  printf("\n\nPower mode testbed\n");
  /*LED GPIO config as output*/
  HAL_GPIO_Pin_Init(USER_LED_1,&gtdPinOutputConfig);
  HAL_GPIO_Pin_Init(USER_LED_2,&gtdPinOutputConfig);
  
  /** 2021/01/18 目前 hal 還沒有定義 GPIO 中斷暫時先呼叫原始 GPIO Driver *****/
  Cy_GPIO_Pin_Init(GPIO_PRT7, 0u, &user_button_port_pin_cfg);
  Cy_SysInt_InitIRQ(&irq_cfg);  /* Register button interrupt for wakeup*/
  Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, ButtonIntHandler);
  NVIC_SetPriority(irq_cfg.intIdx, 3ul);
  NVIC_EnableIRQ(irq_cfg.intIdx); 
  /**************************************************************************/
  
  /** 
  Enable OVD /BOD 設定 
  !!! HAL_PM_Init 不一定要執行 , OVD/BOD 開機預設值 OFF
  */
  HAL_PM_Init(&gts_OVD_BOD_Enable);
   
  for(;;)
  {
#if (SLEEP_POWER_MODE == CURRENT_POWER_MODE)
    SleepModeDemo();
#endif

#if (DEEP_SLEEP_POWER_MODE == CURRENT_POWER_MODE)   
    DeepSleepModeDemo();
#endif
    
#if (HIBERNATE_POWER_MODE == CURRENT_POWER_MODE)
    /*!!! 注意進 Hibernate mode 無法 Debug !!!*/
    /** Reset 或特定I/O 喚醒*/
    HibernateModeDemo();
#endif
    
  }
}