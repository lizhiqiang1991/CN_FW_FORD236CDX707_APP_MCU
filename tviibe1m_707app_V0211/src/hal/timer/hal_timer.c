/***************************************************************************
*\file hal_timer.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "cyhal_hwmgr.h"
#include "hal_timer_def.h"
#include "hal_timer.h"

#define ONE_MEG_HZ     1000000U

static cy_stc_tcpwm_counter_config_t const tsConuterDefConfig =
{
    .period             = 1000U - 1U,                          /* 1000 uSec = 1 mSec */
    .clockPrescaler     = CY_TCPWM_COUNTER_PRESCALER_DIVBY_1,  /* 1,000,000Hz / 1 = 1,000,000 Hz (1 usec)*/
    .runMode            = CY_TCPWM_PWM_CONTINUOUS,
    .countDirection     = CY_TCPWM_COUNTER_COUNT_UP,
    .debug_pause        = 0U,
    .CompareOrCapture   = CY_TCPWM_COUNTER_MODE_COMPARE,
    .compare0           = 0U,
    .compare0_buff      = 0U,
    .compare1           = 0U,
    .compare1_buff      = 0U,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .interruptSources   = 0U,
    .capture0InputMode  = 3U,
    .capture0Input      = 0U,
    .reloadInputMode    = 3U,
    .reloadInput        = 0U,
    .startInputMode     = 3U,
    .startInput         = 0U,
    .stopInputMode      = 3U, /* CY_TCPWM_INPUT_RISING_EDGE  =0 ,CY_TCPWM_INPUT_FALLING_EDGE = 1, CY_TCPWM_INPUT_BOTH_EDGES = 2, CY_TCPWM_INPUT_LEVEL = 3 */
    .stopInput          = 0U,
    .capture1InputMode  = 3U,
    .capture1Input      = 0U,
    .countInputMode     = 3U,
    .countInput         = 1U,
    .trigger1           = CY_TCPWM_COUNTER_OVERFLOW,
};

static uint8_t u8InitDivider = false;

uint8_t HAL_Timer_Init ( uint8_t u8TimerGroup, timer_config_t* tsConfig)
{
  cy_stc_tcpwm_counter_config_t tsCounterConfig;
  uint8_t u8HalResult = DRIVER_REGISTER_ERROR;
  uint32_t u32Status = CY_RET_BAD_PARAM;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  
  if (NULL != pCounter && NULL != tsConfig) 
  {
    /* Assign a programmable divider for TCPWM0_GRP0_CNTx */
    Cy_SysClk_PeriphAssignDivider(pCounter->eClkSrc, 
                                  (cy_en_divider_types_t)CY_SYSCLK_DIV_8_BIT, 
                                  CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT);
    
    if (u8InitDivider == false)
    {
      uint32_t u32PeriFreq = 0;
      uint32_t u32Div = 0;
      Cy_SysClk_GetClkPeriFrequency(&u32PeriFreq);
      u32Div = (u32PeriFreq / ONE_MEG_HZ) - (1U);
      
      #ifdef tviibh8m
      /* Divider 79 --> 80MHz / (79+1) = 1MHz*/
      Cy_SysClk_PeriphSetDivider(
                Cy_SysClk_GetClockGroup(pCounter->eClkSrc),
                (cy_en_divider_types_t)CY_SYSCLK_DIV_8_BIT, 
                CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT, 
                u32Div  /* Divider 79 --> 80MHz / (79+1) = 1MHz*/
                ); 
      Cy_SysClk_PeriphEnableDivider(
                Cy_SysClk_GetClockGroup(pCounter->eClkSrc),
                (cy_en_divider_types_t)CY_SYSCLK_DIV_8_BIT, 
                CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT
                );
      #else
      /* Divider 79 --> 80MHz / (79+1) = 1MHz*/
      Cy_SysClk_PeriphSetDivider((cy_en_divider_types_t)CY_SYSCLK_DIV_8_BIT,
                                 CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT, 
                                 u32Div); 
      Cy_SysClk_PeriphEnableDivider((cy_en_divider_types_t)CY_SYSCLK_DIV_8_BIT, CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT);
      #endif
      u8InitDivider = true;
    }
    else
    {
      u8InitDivider = true;
    }
    /* Initialize TCPWM0_GPR0_CNTx as Timer/Counter */
    memcpy(&tsCounterConfig,&tsConuterDefConfig,sizeof(cy_stc_tcpwm_counter_config_t));
    tsCounterConfig.period  = tsConfig->u32Period - (1U);
    tsCounterConfig.runMode = tsConfig->u8RunType;
    u32Status = Cy_Tcpwm_Counter_Init(pCounter->pBase, &tsCounterConfig);
    if (CY_RET_SUCCESS == u32Status)
    {
      pCounter->u8IsISREnable = tsConfig->u8InterruptEnable;
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_FALSE;
    }
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
    
  return u8HalResult;
}

uint8_t HAL_Timer_DeInit (uint8_t u8TimerGroup)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
   
  if (NULL != pCounter)
  {
    Cy_SysInt_DisableIRQ(pCounter->eIntr);
    Cy_Tcpwm_TriggerStopOrKill(pCounter->pBase);
    Cy_Tcpwm_Counter_Disable  (pCounter->pBase);
    Cy_Tcpwm_Counter_DeInit   (pCounter->pBase);
    pCounter->pCallback = NULL;
    pCounter->pContext  = NULL;
    pCounter->u8IsISREnable = FUNC_DISABLE;
    u8HalResult = DRIVER_TRUE;
  }
  else
    u8HalResult = DRIVER_PARAMETER_ERROR;

  return u8HalResult;
}

uint8_t HAL_Timer_Start(uint8_t u8TimerGroup, uint8_t u8Enable)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  
  if (NULL != pCounter && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
      Cy_Tcpwm_TriggerStart(pCounter->pBase);
    else
      Cy_Tcpwm_TriggerStopOrKill(pCounter->pBase);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Active(uint8_t u8TimerGroup, uint8_t u8Enable)
{
  uint8_t u8HalResult  = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  
  if (NULL != pCounter && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
    {
      /* Enable TC Interrupt mask*/
      Cy_Tcpwm_Counter_SetTC_IntrMask(pCounter->pBase);
      Cy_Tcpwm_Counter_Enable(pCounter->pBase);
    }
    else
      Cy_Tcpwm_Counter_Disable(pCounter->pBase);
    
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Counter_Clear(uint8_t u8TimerGroup)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
   
  if (NULL != pCounter)
  {
    Cy_Tcpwm_Counter_SetCounter(pCounter->pBase,0U);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Read(uint8_t u8TimerGroup, uint32_t * pReturn)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  if ( (NULL != pCounter) && (NULL != pReturn) )
  {
    *pReturn = Cy_Tcpwm_Counter_GetCounter(pCounter->pBase);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Callback_Register (uint8_t u8TimerGroup, Timer_ISR pTimerCallback, void* pContext)
{
  return  CYHAL_TCPWM_IRQ_Callback_Set(u8TimerGroup, pTimerCallback, pContext);
}

uint8_t HAL_Timer_Flag_Clear(uint8_t u8TimerGroup)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  
  if (NULL != pCounter)
  {
    Cy_Tcpwm_Counter_ClearTC_Intr(pCounter->pBase);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Flag_Get(uint8_t u8TimerGroup)
{
  uint8_t u8HalResult = DRIVER_REGISTER_ERROR;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
   
  if (NULL != pCounter)
  {
    if (Cy_Tcpwm_Counter_GetTC_IntrMasked(pCounter->pBase))
      u8HalResult = DRIVER_TRUE;
    else
      u8HalResult = DRIVER_FALSE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}


uint8_t HAL_Timer_Interrupt_Set(uint8_t u8TimerGroup, uint8_t u8Enable, uint8_t u8Priority)
{
  UNUSED(u8Priority);
  
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
   
  if (NULL != pCounter  && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    pCounter->u8IsISREnable = u8Enable;
    if (FUNC_ENABLE == u8Enable)
    {
      Cy_SysInt_EnableIRQ(pCounter->eIntr);
      Cy_Tcpwm_Counter_SetTC_IntrMask(pCounter->pBase);
    }
    else
    {
      Cy_Tcpwm_Counter_ClearTC_Intr(pCounter->pBase);
      Cy_SysInt_DisableIRQ(pCounter->eIntr);
    }
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
     
  return u8HalResult;
}


/*** End of file ***/

  