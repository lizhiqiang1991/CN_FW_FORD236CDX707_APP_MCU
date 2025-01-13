/*

File    : testbed_pwm.c
Purpose : PWM testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the pwm output pin*/
#define PWM_PIN_LINE_38     KIT_LED_4_BLUE  /*PWM OUTPUT TO LED IO_P12_2*/
#define USER_BUTTON         KIT_BUTTON_PIN  /*IO_P7_0*/

#define USER_PWM_PIN_LINE   PWM_PIN_LINE_38
#define USER_PWM_GROUP      PWM_GROUP0_CNT38

#define USER_LED            KIT_LED_1_BLUE

/*PWM Configuration*/
static pwm_config_t ts_1K_hz_Config = 
{
  /*
  Peri clock = 80Mhz
  u8PreScaler = 8 , 80Mhz/8 = 10Mhz
  Period = 10,000 , 10Mhz / 10,000 = 1Khz 
  Compare = 100  , 1% PWM duty (100/10,000)
  PWM OUTPUT => 1Khz , 1% duty 
  */          
  .u8PreScaler        = PWM_PRESCALER_DIVBY_8,  /**< 1/2/4/8/16/32/64/128 */
  .u8Pin              = USER_PWM_PIN_LINE,      /**< pwm output pin*/
  .u8PinComplementary = IO_NC,                  /**< An optional, additional inverted output pin.*/
  .u16Period          = 10000U,                 /**< 0 ~ 65535*/
  .u16Duty            = 100U,                   /**< 0 ~ 65535 Compare */
  .u8Mode             = PWM_LEFT_ALIGN,         /**< PWM alignment */
  .u8RunType          = PWM_CONTINUOUS,         /**< Continuous or One Shot*/ 
  .u8Invert           = FUNC_DISABLE,           /**< An option for the user to invert the PWM output*/
  .u8InterruptEnable  = FUNC_DISABLE,           /**< Enable/Disable Interrupt*/
  .u8NvicPriority     = 0U,                     /**< ISR priority*/
  .u32Timeout         = 0U,                     /**< PWM Config timeout */
};

static void PWM_ISR_Handler(void* context)
{
  UNUSED (context);
  static uint16_t count = 0;
  /* Toggle LED after 1000 PWM pulses */
  if (count >= 1000)
  {
    count = 0;
    HAL_GPIO_PinToggle(USER_LED);
  }
  else
  {
    count ++;
  }
}

void testbed_pwm(void) 
{
  __enable_irq();
  
  /*LED GPIO config as output*/
  HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
  
  /*Button GPIO config as input*/
  HAL_GPIO_Pin_Init(USER_BUTTON,&gtdPinInputConfig);
  
  /*Init , active and start PWM*/
  HAL_PWM_Init  (USER_PWM_GROUP, &ts_1K_hz_Config);
  HAL_PWM_Active(USER_PWM_GROUP, FUNC_ENABLE);
  HAL_PWM_Start (USER_PWM_GROUP, FUNC_ENABLE);
  
  /*It is not necessary to register pwm callback demo olny*/
  HAL_PWM_Callback_Register(USER_PWM_GROUP,PWM_ISR_Handler,NULL);
  
  uint8_t  u8Button = 0;
  uint16_t u16LedDuty = 0;
  uint16_t u16Period = 0;
  
  /*Get current duty/ period*/
  HAL_PWM_Duty_Get  (USER_PWM_GROUP,&u16LedDuty);
  HAL_PWM_Period_Get(USER_PWM_GROUP,&u16Period);
  
  uint8_t  u8Percent = 0;
  for (;;)
  {  
    HAL_GPIO_PinRead(USER_BUTTON,&u8Button); 
    if (0U == u8Button)
    {
        Cy_SysLib_Delay(100); /*Delay 100 msec , simple button debouncing*/
        HAL_GPIO_PinRead(USER_BUTTON,&u8Button);
    }
    else
    {
      Cy_SysLib_Delay(10);
      continue;
    }
    
    if (0U == u8Button /* Button Pressed*/) 
    {
      /*Disable PWM interrupt (PWM_ISR_Handler)*/
      HAL_PWM_Interrupt_Set(USER_PWM_GROUP,FUNC_DISABLE,CYHAL_TCPWM_IRQ_PRIORITY);
      HAL_GPIO_PinWrite(USER_LED,PIN_LOW);
      
      u16LedDuty += 800U;
     
      if (u16LedDuty >= u16Period)
      {
        u16LedDuty = 100U;
      }
      /* Use pwm for LED dimming*/
      u8Percent = (uint8_t)((float) ( (float)u16LedDuty/(float)u16Period ) * 100.0);
      printf("PWM Duty = %d , %d%%\n",u16LedDuty,u8Percent);
      HAL_PWM_Duty_Set(USER_PWM_GROUP,u16LedDuty);
    }
  }
}

