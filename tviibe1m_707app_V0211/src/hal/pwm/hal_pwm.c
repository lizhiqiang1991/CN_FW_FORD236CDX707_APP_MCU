/***************************************************************************
*\file hal_pwm.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "cyhal_hwmgr.h"
#include "hal_gpio_def.h"
#include "hal_gpio.h"
#include "hal_pwm_def.h"
#include "hal_pwm.h"

static uint8_t u8Init_PWM_Divider = false;

// #define HAL_PWM_AUTO_RELOAD 20210603 mail

#ifdef HAL_PWM_AUTO_RELOAD
  #define HAL_PWM_DUTY_SET(PWM_BASE,DUTY)   Cy_Tcpwm_Pwm_SetCompare0_Buff((PWM_BASE),(DUTY))
  #define HAL_PWM_DUTY_GET(PWM_BASE)        Cy_Tcpwm_Pwm_GetCompare0_Buff((PWM_BASE))
  #define HAL_PWM_PERIOD_SET(PWM_BASE,PERI) Cy_Tcpwm_Pwm_SetPeriodBuf((PWM_BASE),(PERI))
  #define HAL_PWM_PERIOD_GET(PWM_BASE)      Cy_Tcpwm_Pwm_GetPeriodBuf((PWM_BASE))
  #pragma message("\t\t(HAL_PWM Enable auto reload)") 
#else
  #define HAL_PWM_DUTY_SET(PWM_BASE,DUTY)   Cy_Tcpwm_Pwm_SetCompare0((PWM_BASE),(DUTY))
  #define HAL_PWM_DUTY_GET(PWM_BASE)        Cy_Tcpwm_Pwm_GetCompare0((PWM_BASE))
  #define HAL_PWM_PERIOD_SET(PWM_BASE,PERI) Cy_Tcpwm_Pwm_SetPeriod((PWM_BASE),(PERI)) 
  #define HAL_PWM_PERIOD_GET(PWM_BASE)      Cy_Tcpwm_Pwm_GetPeriod((PWM_BASE))
  #pragma message("\t\t(HAL_PWM DISABLE auto reload)") 
#endif

static uint8_t TCPWM_Source_Clock_Init(cy_tcpwm_t* pPWM)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_en_sysclk_status_t eClk_Status = CY_SYSCLK_INVALID_STATE;
  
  /* Assign a programmable divider for TCPWM , every TCPWM in common clock group*/
  eClk_Status = Cy_SysClk_PeriphAssignDivider(pPWM->eClkSrc,
                                             (cy_en_divider_types_t)CY_SYSCLK_DIV_8_BIT,
                                              CYHAL_DIV_NUM_TCPWM_PWM_8_BIT);
  if (CY_SYSCLK_SUCCESS == eClk_Status)
  {
    if (false == u8Init_PWM_Divider)
    {
      /*According to the current peripheral CLOCK frequency(CLK_HF0), the PWM source frequency is not divided*/
      Cy_SysClk_PeriphSetDivider((cy_en_divider_types_t)CY_SYSCLK_DIV_8_BIT,
                                  CYHAL_DIV_NUM_TCPWM_PWM_8_BIT, 0U); 
      Cy_SysClk_PeriphEnableDivider((cy_en_divider_types_t)CY_SYSCLK_DIV_8_BIT, CYHAL_DIV_NUM_TCPWM_PWM_8_BIT);
      u8Init_PWM_Divider = true;
    }
    
    u8HalResult = DRIVER_TRUE;
   
  }
  else
  {
    u8HalResult = DRIVER_WRITE_ERROR;
  }
  
  return u8HalResult;
}

static uint8_t TCPWM_PIN_Init( uint8_t u8Pin , uint8_t u8PinComplementary)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  gpio_pin_config_t tsGpioCfg = 
  {
    .u8OutValue =  1U,                          /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF,    /**< Drive mode (OUTPUT)*/
    .u8HSIOM = HSIOM_SEL_ACT_0,                 /**< HSIOM selection tcpwm[0].line[x]*/
    .u8Vtrip = 0U,                              /**< Input buffer voltage trip type */
    .u8SlewRate = 0U,                           /**< Output buffer slew rate */
    .u8DriveStength = 0U,                       /**< Drive strength */
  };
  
  /* Port Configuration for PWM */
  u8HalResult = HAL_GPIO_Pin_Init(u8Pin, &tsGpioCfg);
  if (DRIVER_TRUE == u8HalResult)
  {
    if (IO_NC != u8PinComplementary)
    {
      tsGpioCfg.u8HSIOM = HSIOM_SEL_ACT_1; /**< tcpwm[0].line_compl[x]*/
      u8HalResult = HAL_GPIO_Pin_Init(u8PinComplementary, &tsGpioCfg);
    }
    else
    {
      u8HalResult = DRIVER_TRUE;
    }
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

static uint8_t TCPWM_Config_Init( volatile stc_TCPWM_GRP_CNT_t* ptcpwm, pwm_config_t* tsConfig)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  uint32_t u32Status = CY_RET_BAD_PARAM;
  cy_stc_tcpwm_pwm_config_t tsPWM_config = {0U};
  
  tsPWM_config.pwmMode            = CY_TCPWM_PWM_MODE_PWM;
  tsPWM_config.clockPrescaler     = tsConfig->u8PreScaler;
  tsPWM_config.Cc0MatchMode       = CY_TCPWM_PWM_TR_CTRL2_CLEAR;
  tsPWM_config.OverflowMode       = CY_TCPWM_PWM_TR_CTRL2_SET;
  tsPWM_config.UnderflowMode      = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE;
  tsPWM_config.Cc1MatchMode       = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE;
  tsPWM_config.runMode            = tsConfig->u8RunType; 
  tsPWM_config.period             = (uint32_t)tsConfig->u16Period;  
  tsPWM_config.compare0           = (int32_t)tsConfig->u16Duty;  
  tsPWM_config.invertPWMOut       = tsConfig->u8Invert;
  tsPWM_config.invertPWMOutN      = tsConfig->u8Invert;
  tsPWM_config.killMode           = CY_TCPWM_PWM_STOP_ON_KILL,
  tsPWM_config.switchInputMode    = CY_TCPWM_INPUT_LEVEL;
  tsPWM_config.reloadInputMode    = CY_TCPWM_INPUT_LEVEL;
  tsPWM_config.startInputMode     = CY_TCPWM_INPUT_LEVEL;
  tsPWM_config.kill0InputMode     = CY_TCPWM_INPUT_LEVEL;
  tsPWM_config.kill1InputMode     = CY_TCPWM_INPUT_LEVEL;
  tsPWM_config.countInputMode     = CY_TCPWM_INPUT_LEVEL;
  tsPWM_config.countInput         = 1U;
  
#ifdef HAL_PWM_AUTO_RELOAD  
  /*
    CC0 and PERIOD Auto Reload with Switch Event
    Auto CC reload and auto PERIOD reload will provide dynamic PWM duty cycle change 
    and count period length change. The active switch event (capture0) is required for switch activity:
  */
  tsPWM_config.switchInput        = 1ul;
  tsPWM_config.enableCompare0Swap = true; /*Auto CC0 reload*/
  tsPWM_config.enablePeriodSwap   = true; /*Auto Period reload*/
  tsPWM_config.period_buff        = (uint32_t)tsConfig->u16Period; ;
#endif
  
  /* 
    Keep future expansion requirements
    tsPWM_config.debug_pause        = false;
    tsPWM_config.deadTime           = 0U;
    tsPWM_config.deadTimeComp       = 0U;
    tsPWM_config.compare1           = 0ul;
    tsPWM_config.enableCompare1Swap = false;
    tsPWM_config.interruptSources   = 0ul;
    tsPWM_config.startInput         = 0ul;
    tsPWM_config.reloadInput        = 0ul;
    tsPWM_config.kill0Input         = 0ul;
    tsPWM_config.kill1Input         = 0ul;
  */
  
  /* Initialize PWM */
  u32Status = Cy_Tcpwm_Pwm_Init(ptcpwm, &tsPWM_config);
  if (CY_RET_SUCCESS == u32Status)
  {
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

static uint8_t HAL_PWM_Config_Check(cy_tcpwm_t* pPWM , pwm_config_t* tsConfig)                           
{
    uint8_t u8HalResult = DRIVER_FALSE;
    if ((NULL != tsConfig) && (NULL != pPWM))
    {
        if ( (tsConfig->u8PreScaler == PWM_PRESCALER_DIVBY_1    ||\
              tsConfig->u8PreScaler == PWM_PRESCALER_DIVBY_2    ||\
              tsConfig->u8PreScaler == PWM_PRESCALER_DIVBY_4    ||\
              tsConfig->u8PreScaler == PWM_PRESCALER_DIVBY_8    ||\
              tsConfig->u8PreScaler == PWM_PRESCALER_DIVBY_16   ||\
              tsConfig->u8PreScaler == PWM_PRESCALER_DIVBY_32   ||\
              tsConfig->u8PreScaler == PWM_PRESCALER_DIVBY_64   ||\
              tsConfig->u8PreScaler == PWM_PRESCALER_DIVBY_128) &&\
             (tsConfig->u8Mode == PWM_LEFT_ALIGN                ||\
              tsConfig->u8Mode == PWM_RIGHT_ALIGN               ||\
              tsConfig->u8Mode == PWM_CENTER_ALIGN)             &&\
             (tsConfig->u8RunType == PWM_ONESHOT                ||\
              tsConfig->u8RunType == PWM_CONTINUOUS)            \
            )
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}
    
uint8_t HAL_PWM_Init( uint8_t u8PwmGroup, pwm_config_t* tsConfig)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
  if (DRIVER_TRUE == HAL_PWM_Config_Check(pPWM,tsConfig))
  {
    if (DRIVER_TRUE == TCPWM_Config_Init(pPWM->pBase,tsConfig))
    {
      /* Port Configuration for PWM */
      u8HalResult = TCPWM_PIN_Init(tsConfig->u8Pin,tsConfig->u8PinComplementary);
      if (DRIVER_TRUE == u8HalResult)
      {
        u8HalResult = TCPWM_Source_Clock_Init(pPWM);
      }
      else
      {
        u8HalResult = DRIVER_REGISTER_ERROR;
      }
    }
    else
    {
      u8HalResult = DRIVER_WRITE_ERROR;
    }
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
 
  return u8HalResult;
}

uint8_t HAL_PWM_DeInit( uint8_t u8PwmGroup)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
  if (NULL != pPWM)
  {
    Cy_Tcpwm_TriggerStopOrKill(pPWM->pBase);
    Cy_Tcpwm_Pwm_Disable(pPWM->pBase);
    Cy_Tcpwm_Pwm_DeInit(pPWM->pBase);
    pPWM->pCallback  = NULL;
    pPWM->pContext   = NULL;
    pPWM->u8IsISREnable = FUNC_DISABLE;
    
    u8HalResult = DRIVER_TRUE;
  }
  else
    u8HalResult = DRIVER_PARAMETER_ERROR;

  return u8HalResult;  
}
 
uint8_t HAL_PWM_Start( uint8_t u8PwmGroup, uint8_t u8Enable)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
   
  if (NULL != pPWM && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
      Cy_Tcpwm_TriggerStart(pPWM->pBase);
    else
      Cy_Tcpwm_TriggerStopOrKill(pPWM->pBase);
    
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_PWM_Active( uint8_t u8PwmGroup, uint8_t u8Enable)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
   
  if (NULL != pPWM && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
    {
      /* Enable CC0 Interrupt mask*/
      Cy_Tcpwm_Counter_SetCC0_IntrMask(pPWM->pBase);
      Cy_Tcpwm_Pwm_Enable(pPWM->pBase);
    }
    else
    {
      Cy_Tcpwm_Pwm_Disable(pPWM->pBase);
    }
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_PWM_Duty_Set( uint8_t u8PwmGroup, uint16_t u16Duty)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
   
  if (NULL != pPWM)
  {
     
     HAL_PWM_DUTY_SET(pPWM->pBase,(uint32_t)u16Duty);
     u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_PWM_Duty_Get( uint8_t u8PwmGroup, uint16_t* pReturn)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
  if ((NULL != pPWM) && (NULL != pReturn))
  {
     *pReturn = (uint16_t)HAL_PWM_DUTY_GET(pPWM->pBase);
     u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_PWM_Period_Set( uint8_t u8PwmGroup, uint16_t u16Period)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
  if (NULL != pPWM)
  {
     HAL_PWM_PERIOD_SET(pPWM->pBase,u16Period);
     u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_PWM_Period_Get( uint8_t u8PwmGroup, uint16_t* pReturn)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
  if ((NULL != pPWM) && (NULL != pReturn))
  {
     *pReturn = (uint16_t)HAL_PWM_PERIOD_GET(pPWM->pBase);
     u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_PWM_Output_Invert( uint8_t u8PwmGroup, uint8_t u8Enable)
{
  uint8_t u8HalResult = DRIVER_PARAMETER_ERROR;
  UNUSED(u8PwmGroup);
  UNUSED(u8Enable);
  /*Currently SDL 6.5.0 does not implement PWM invert output*/
  return u8HalResult;
}

uint8_t HAL_PWM_Interrupt_Set( uint8_t u8PwmGroup, uint8_t u8Enable, uint8_t u8Priority)
{
  UNUSED(u8Priority);
  
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pPWM = CYHAL_TCPWM_Get(u8PwmGroup);
   
  if ((NULL != pPWM) && (FUNC_ENABLE == u8Enable || FUNC_DISABLE == u8Enable))
  {
    pPWM->u8IsISREnable = u8Enable;
    if (FUNC_ENABLE == u8Enable)
    {
      Cy_SysInt_EnableIRQ(pPWM->eIntr);
      /* Enable CC0 Interrupt mask*/
      Cy_Tcpwm_Counter_SetCC0_IntrMask(pPWM->pBase);
    }
    else
    {
      Cy_Tcpwm_Counter_ClearCC0_Intr(pPWM->pBase);
      Cy_SysInt_DisableIRQ(pPWM->eIntr);
    }
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
     
  return u8HalResult;
}

uint8_t HAL_PWM_Callback_Register (uint8_t u8PwmGroup, PWM_ISR pPwmCallback, void* pContext)
{
  return CYHAL_TCPWM_IRQ_Callback_Set(u8PwmGroup, pPwmCallback, pContext);
}

/*** End of file ***/

  