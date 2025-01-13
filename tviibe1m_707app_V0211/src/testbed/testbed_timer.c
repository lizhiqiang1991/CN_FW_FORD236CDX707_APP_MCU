/*

File    : testbed_timer.c
Purpose : Timer testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"
 
#ifdef tviibh8m
  #define USER_LED        IO_P23_2
#else
  #define USER_LED        KIT_LED_1_BLUE //IO_P7_1
#endif

#define LED_TIMER           TIMER_GROUP0_CNT0
#define MEASURE_TIMER       TIMER_GROUP0_CNT1
#define FIFTY_mSec_TIMER    TIMER_GROUP0_CNT2
#define COUNTER_FLAG_TIMER  TIMER_GROUP0_CNT4

static   uint16_t u16Count = 0u;
static   uint16_t u16Timeout_printf = false;
static   uint8_t  u8BlinkTimes = 0u;

void LED_Timer_Handler(void* context)
{
   UNUSED (context);

   if (u16Count >= 1000u) /* Interrupt every Millisecond , 1000 msec = 1 sec*/
   {
       HAL_GPIO_PinToggle(USER_LED);
       //printf("led\n");
       u16Count = 0;
       u8BlinkTimes ++;
   }
   else
     u16Count++;
}

void Timer_Handler_50mSec (void* context)
{
  uint8_t data = *((uint8_t*)context);
  if (false == u16Timeout_printf)
  {
    printf("Timer_Handler_50mSec context = 0x%x\n",data);
    u16Timeout_printf = true;
  }
  //UNUSED(data);
}

void testbed_timer(void) 
{
  static uint8_t dummy_data = 0x55;
 
  __enable_irq();
   
  timer_config_t ts_50mSec_Timer = 
  {
    .u32Period = 50000u,          /*0~ 65535 uSec*/
    .u8RunType = TIMER_CONTINUOUS,/* Continuous or One Shot*/
  } ;

  /*LED gpio init config as output*/
  HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
  
  /*Timer init*/
  HAL_Timer_Init(LED_TIMER          ,&gts_1mSec_Timer_Continuous_Config); /*Timer Continuous*/
  HAL_Timer_Init(MEASURE_TIMER      ,&gts_1mSec_Timer_OneShot_Config);    /*Timer One Shot*/
  HAL_Timer_Init(FIFTY_mSec_TIMER   ,&ts_50mSec_Timer);
  HAL_Timer_Init(COUNTER_FLAG_TIMER ,&ts_50mSec_Timer);
  
  /*Timer register callback*/
  HAL_Timer_Callback_Register (LED_TIMER,LED_Timer_Handler,NULL);
  HAL_Timer_Callback_Register (FIFTY_mSec_TIMER,Timer_Handler_50mSec,(void*)&dummy_data);

  /*Timer Enable*/
  HAL_Timer_Active(LED_TIMER          ,FUNC_ENABLE);
  HAL_Timer_Active(MEASURE_TIMER      ,FUNC_ENABLE);
  HAL_Timer_Active(FIFTY_mSec_TIMER   ,FUNC_ENABLE);
  HAL_Timer_Active(COUNTER_FLAG_TIMER ,FUNC_ENABLE);
  
  /*LED_TIMER & COUNTER_FLAG_TIMER timer start*/ 
  HAL_Timer_Start(LED_TIMER          ,FUNC_ENABLE);
  HAL_Timer_Start(COUNTER_FLAG_TIMER ,FUNC_ENABLE);
  
  /*Demo HAL_Timer_Flag_Get / Set*/
  for (;;)
  {
    if (DRIVER_TRUE == HAL_Timer_Flag_Get(COUNTER_FLAG_TIMER))
    {
      printf("(COUNTER_FLAG_TIMER)50 mSec Timeout \n");
      /* clear flag*/
      HAL_Timer_Flag_Clear(COUNTER_FLAG_TIMER); 
      break;
    }
  }
  
  /*MEASURE timer start*/
  HAL_Timer_Start(MEASURE_TIMER      ,FUNC_ENABLE);
  /* Delay some time to measure time */
  Cy_SysLib_DelayUs(30); /*Delay 30 usec , counter default period 1000 usec (MAX)*/
  uint32_t delayTime = 0;
  HAL_Timer_Read(MEASURE_TIMER,&delayTime);
  printf("Time = %d usec\n",delayTime); /*Dispaly 31 usec*/
  /*Close measurement timer*/
  HAL_Timer_DeInit(MEASURE_TIMER);

  /* reinit & start*/
  HAL_Timer_Init  (MEASURE_TIMER,&gts_1mSec_Timer_OneShot_Config);
  HAL_Timer_Active(MEASURE_TIMER,FUNC_ENABLE);
  HAL_Timer_Start (MEASURE_TIMER,FUNC_ENABLE);
  
  /* Delay some time to measure time */
  Cy_SysLib_DelayUs(60); /*Delay 60 usec*/
  HAL_Timer_Read(MEASURE_TIMER,&delayTime);
  printf("Time = %d usec\n",delayTime); /*Dispaly 61 usec*/
  HAL_Timer_Active(MEASURE_TIMER,FUNC_DISABLE);
 
  /*Other Timer start*/
  HAL_Timer_Start(FIFTY_mSec_TIMER , FUNC_ENABLE);
  for (;;)
  {
    if (u8BlinkTimes >= 6u)
    {
      /* Disable LED_TIMER interrupt*/
      HAL_Timer_Interrupt_Set (LED_TIMER, FUNC_DISABLE, CYHAL_TCPWM_IRQ_PRIORITY);
      /*Clear LDE_TIMER counter*/
      HAL_Timer_Counter_Clear(LED_TIMER);
      u16Count = 0;
      
      /* Delay 5 second */
      Cy_SysLib_Delay(5000); 
      
      /*Enable LED_TIMER interrupt & recounting */
      HAL_Timer_Interrupt_Set (LED_TIMER, FUNC_ENABLE, CYHAL_TCPWM_IRQ_PRIORITY);
      u8BlinkTimes = 0u;
    }
  }
}
