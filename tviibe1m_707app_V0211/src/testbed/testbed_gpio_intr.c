/*

File    : testbed_gpio_intr.c
Purpose : GPIO interrupt testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include "hal_gpio_def.h"
#include "CYTVII_B_E_1M_KIT_PIN.h"

#define USER_LED        KIT_LED_1_BLUE 
#define USER_BUTTON     KIT_BUTTON_PIN    /*SW1 CYTVII-B-E-1M-SK*/

#if GPIO_INTERRUPT_TEST 
static void Button_ISR(void)
{
    HAL_GPIO_PinToggle(USER_LED);
}
#endif

void testbed_gpio_intr(void) 
{
  __enable_irq();
  
   /*LED GPIO config as output*/
  HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
  
  /*BUTTON GPIO config as input*/
  HAL_GPIO_Pin_Init(USER_BUTTON,&gtdPinInputConfig);
#if GPIO_INTERRUPT_TEST  
  /*Register button GPIO interrupt*/
  HAL_GPIO_Callback_Register(USER_BUTTON, CY_GPIO_INTR_FALLING , Button_ISR);
#endif  
  for (;;);
}


/*************************** End of file ****************************/
