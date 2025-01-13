/***************************************************************************
*\file hal_clock.c
****************************************************************************/ 

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_clock_def.h"
#include "hal_clock.h"

#define FLASH_WAIT_STATE     (1ul) /* divided by 2 */
#define RAM0_FAST_WAIT_STATE (0ul) /* divided by 1 */
#define RAM0_SLOW_WAIT_STATE (1ul) /* divided by 2 */
#define RAM1_FAST_WAIT_STATE (0ul) /* divided by 1 */
#define RAM1_SLOW_WAIT_STATE (1ul) /* divided by 2 */
#define ROM_FAST_WAIT_STATE  (0ul) /* divided by 1 */
#define ROM_SLOW_WAIT_STATE  (1ul) /* divided by 2 */

#define CLK_FAST_DIVIDER  (1ul)  /* divided by 2 */
#define CLK_SLOW_DIVIDER  (0ul)  /* no division  */
#define CLK_PERI_DIVIDER  (1ul)  /* divided by 2 */

#define CLK_FREQ_IMO        ( 8000000ul)

#define FLL_TARGET_FREQ    (100000000ul)
#define PLL_TARGET_FREQ    (160000000ul)

static const cy_stc_pll_config_t tsPLL_SOURCE_ECO_Config = 
{
    .inputFreq  = CLK_FREQ_ECO,       /* ECO: 16MHz*/
    .outputFreq = PLL_TARGET_FREQ,    /* target PLL output*/
    .lfMode     = 0u,                 /* VCO frequency is [200MHz, 400MHz]*/
    .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
};

static const cy_stc_pll_config_t tsPLL_SOURCE_IMO_Config = 
{
    .inputFreq  = CLK_FREQ_IMO,       /* IMO: 8MHz */
    .outputFreq = PLL_TARGET_FREQ,    /* target PLL output */
    .lfMode     = 0u,                 /* VCO frequency is [200MHz, 400MHz]*/
    .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
};

static uint8_t CYHAL_PLL_Setting(clock_config_t* tsConfig)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_en_sysclk_status_t eStatus;
  cy_en_clkpath_in_sources_t eClkPath_IN;
  const cy_stc_pll_config_t* ptsPLL_Config;
  if (FUNC_ENABLE == tsConfig->u8EcoEnable)
  {
    eClkPath_IN = CY_SYSCLK_CLKPATH_IN_ECO;
    ptsPLL_Config = &tsPLL_SOURCE_ECO_Config;
  }
  else
  {
    eClkPath_IN = CY_SYSCLK_CLKPATH_IN_IMO;
    ptsPLL_Config = &tsPLL_SOURCE_IMO_Config;
  }
  
  Cy_SysClk_ClkPathSetSource(PLL_PATH_NO, eClkPath_IN);
  Cy_SysClk_PllConfigure(PLL_PATH_NO , ptsPLL_Config);
  eStatus = Cy_SysClk_PllEnable(PLL_PATH_NO, tsConfig->u32Timeout);
  if (CY_SYSCLK_SUCCESS == eStatus)
    u8HalResult = DRIVER_TRUE;
  else
    u8HalResult = DRIVER_TIMEOUT;
  return u8HalResult;
}

static uint8_t CYHAL_ECO_Setting(uint32_t u32Timeout)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  uint8_t u8ECO_OK = false;
  uint8_t u8ECO_Ready = false;
   
  /***    ECO port settings        ***/
  /* Default settings should be OK. */
  
  /***    ECO setting and enabling        ***/
  /* These values need to be confirmed */
  SRSS->unCLK_ECO_CONFIG2.stcField.u3WDTRIM = 4u;
  SRSS->unCLK_ECO_CONFIG2.stcField.u4ATRIM  = 12u;
  SRSS->unCLK_ECO_CONFIG2.stcField.u2FTRIM  = 3u;
  SRSS->unCLK_ECO_CONFIG2.stcField.u2RTRIM  = 3u;
  SRSS->unCLK_ECO_CONFIG2.stcField.u3GTRIM  = 1u;
  
  SRSS->unCLK_ECO_CONFIG.stcField.u1ECO_EN = 1ul;
  
  /*Check ECO OK*/
  for (uint32_t i = 0 ; i < u32Timeout ; i++)
  {
    if (0u == SRSS->unCLK_ECO_STATUS.stcField.u1ECO_OK)
    {
      u8ECO_OK = true;
      break;
    }
  }
  
  /*Check ECO Ready*/
  for (uint32_t i = 0 ; i < u32Timeout ; i++)
  {
    if (0u == SRSS->unCLK_ECO_STATUS.stcField.u1ECO_READY)
    {
      u8ECO_Ready = true;
      break;
    }
  }
  
  if ((true == u8ECO_OK) && (true == u8ECO_Ready))
    u8HalResult = DRIVER_TRUE;
  else
    u8HalResult = DRIVER_TIMEOUT;
  
  return u8HalResult;
}

static uint8_t CYHAL_Clock_Init(clock_config_t* tsConfig)
{ 
  uint8_t u8HalResult = DRIVER_FALSE;
  
  /***  Set CPUSS dividrs as required        ***/
  /* FAST = PERI = CLK_SLOW = 80,000,000 */
  Cy_SysClk_FastClkSetDivider (CLK_FAST_DIVIDER);
  Cy_SysClk_PeriClkSetDivider (CLK_PERI_DIVIDER);
  Cy_SysClk_SlowClkSetDivider (CLK_SLOW_DIVIDER);
  
  /***     PLL setting and enabling        ***/
  if (DRIVER_TIMEOUT == CYHAL_PLL_Setting(tsConfig))
  {
    u8HalResult = DRIVER_TIMEOUT;
  }
  else
  {
    /***  Assign  PLL0 as source of clk_hf0        ***/
    /* Select source of clk_hf0 */
    /***  Set HF source, divider, enable   ***/
    Cy_SysClk_HfClockSetSource(CY_SYSCLK_HFCLK_0,CY_SYSCLK_HFCLK_IN_CLKPATH1);
    Cy_SysClk_HfClockSetDivider(CY_SYSCLK_HFCLK_0,CY_SYSCLK_HFCLK_NO_DIVIDE);
    Cy_SysClk_HfClkEnable(CY_SYSCLK_HFCLK_0);
    
    /***   Setting  PATH2  source        ***/
    Cy_SysClk_ClkPathSetSource(2u,CY_SYSCLK_CLKPATH_IN_IMO);
    
    /***     FLL  disabling        ***/
    /* Disable Fll */
    Cy_SysClk_FllDisable();
    
    /***     Enabling ILO0        ***/
    Cy_WDT_Unlock();
    Cy_SysClk_Ilo0Enable();
    Cy_SysClk_Ilo0HibernateOn(1u);
    Cy_WDT_Lock();
    
    /* Update core clock info*/
    SystemCoreClockUpdate();
    u8HalResult = DRIVER_TRUE;
  }
  return u8HalResult;
}

uint8_t HAL_Clock_Init(clock_config_t *tsConfig)
{
  /*ARM M0 pluse*/
#if (__CORTEX_M == 0x0u)
  
  uint8_t u8HalResult = DRIVER_FALSE;
  if (NULL == tsConfig)
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  else
  {
    /* disable WDT */
    Cy_WDT_Disable();
    
    /*********** Setting wait state for ROM **********/
    CPUSS->unROM_CTL.stcField.u2SLOW_WS = ROM_SLOW_WAIT_STATE;
    CPUSS->unROM_CTL.stcField.u2FAST_WS = ROM_FAST_WAIT_STATE;
    
    /*********** Setting wait state for RAM **********/
    CPUSS->unRAM0_CTL0.stcField.u2SLOW_WS = RAM0_SLOW_WAIT_STATE;
    CPUSS->unRAM0_CTL0.stcField.u2FAST_WS = RAM0_FAST_WAIT_STATE;
    
    CPUSS->unRAM1_CTL0.stcField.u2SLOW_WS = RAM1_SLOW_WAIT_STATE;
    CPUSS->unRAM1_CTL0.stcField.u2FAST_WS = RAM1_FAST_WAIT_STATE;
    
    /*********** Setting wait state for FLASH **********/
    FLASHC->unFLASH_CTL.stcField.u4MAIN_WS = FLASH_WAIT_STATE;
    
    /***    Set clock LF source  (WDT)      ***/
    SRSS->unCLK_SELECT.stcField.u3LFCLK_SEL = CY_SYSCLK_LFCLK_IN_ILO0;
    
    /***** ECO setting ******/
    if (FUNC_ENABLE == tsConfig->u8EcoEnable)
    {
      u8HalResult = CYHAL_ECO_Setting(tsConfig->u32Timeout);
    } /*end if ECO Setting*/
    
    if ((FUNC_ENABLE == tsConfig->u8EcoEnable) && (DRIVER_TRUE != u8HalResult))
      u8HalResult = DRIVER_TIMEOUT;
    else
      u8HalResult = CYHAL_Clock_Init(tsConfig);
    
    /* Check the IO status. If current status is frozen, unfreeze the system. */
    if(Cy_SysPm_GetIoFreezeStatus())
    {
      /* Unfreeze the system */
      Cy_SysPm_IoUnfreeze();
    }
    else
    {
      ;/* Do nothing */
    }
  }
  return u8HalResult;
  
#else
   /*ARM M4 : ARM M0 has initialized the system clock*/
   UNUSED(tsConfig);
   SystemInit();  /*M4 system init*/
   /* Check the IO status. If current status is frozen, unfreeze the system. */
   if(Cy_SysPm_GetIoFreezeStatus())
   {
     /* Unfreeze the system */
     Cy_SysPm_IoUnfreeze();
   }
   else
   {
     ;/* Do nothing */
   }
   return DRIVER_TRUE;
#endif
}

#if 0
uint8_t HAL_Clock_Init_old(clock_config_t *tsConfig)
{
  UNUSED(tsConfig);
  /* Call SDL SystemInit temporarily because the settings of m0 and m4 are different. */
  SystemInit();
  
  /* Check the IO status. If current status is frozen, unfreeze the system. */
  if(Cy_SysPm_GetIoFreezeStatus())
  {
      /* Unfreeze the system */
      Cy_SysPm_IoUnfreeze();
  }
  else
  {
      ;/* Do nothing */
  }
  /* and the FLL or PLL has ECO as a source
  #if CY_CPU_CORTEX_M4
  cy_en_clkpath_in_sources_t eClkSource_ClkPath0 = Cy_SysClk_ClkPathGetSource(CLOCK_PATH0);
  cy_en_clkpath_in_sources_t eClkSource_ClkPath1 = Cy_SysClk_ClkPathGetSource(CLOCK_PATH1);
  printf("clk path0 %d , clk path1 %d\n",eClkSource_ClkPath0,eClkSource_ClkPath1);
  */
  return DRIVER_TRUE;
}
#endif

uint8_t HAL_Clock_PLL_Configuration(pll_fll_config_t *tsPllConfig)
{
  cy_en_sysclk_status_t eResult = CY_SYSCLK_INVALID_STATE;
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_stc_pll_config_t tsCyPllConfig = {(0U)};
  
  if (NULL != tsPllConfig)
  {
    eResult = Cy_SysClk_ClkPathSetSource( PLL_PATH_NO, (cy_en_clkpath_in_sources_t)tsPllConfig->u8InputClockPath);
    /*CY_ASSERT(eResult == CY_SYSCLK_SUCCESS); fatal error*/
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      tsCyPllConfig.inputFreq  = tsPllConfig->u32InClkFreq;
      tsCyPllConfig.outputFreq = tsPllConfig->u32OutClkFreq;
      tsCyPllConfig.lfMode     = (0U);  /**< CLK_PLL_CONFIG register, PLL_LF_MODE bit */
      tsCyPllConfig.outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO;
      eResult = Cy_SysClk_PllConfigure(PLL_PATH_NO , &tsCyPllConfig);
      if (CY_SYSCLK_SUCCESS == eResult)
      {
        eResult = Cy_SysClk_PllEnable(PLL_PATH_NO, WAIT_FOR_STABILIZATION);
        if (CY_SYSCLK_SUCCESS == eResult)
        {
          u8HalResult = DRIVER_TRUE;
        }
        else
        {
          u8HalResult = DRIVER_TIMEOUT;
        }
      }
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

uint8_t HAL_Clock_FLL_Configuration(pll_fll_config_t *tsFllConfig)
{
  cy_en_sysclk_status_t eResult = CY_SYSCLK_INVALID_STATE;
  uint8_t u8HalResult = DRIVER_FALSE;
  
  if (NULL != tsFllConfig)
  {
    eResult = Cy_SysClk_ClkPathSetSource( FLL_PATH_NO, (cy_en_clkpath_in_sources_t)tsFllConfig->u8InputClockPath);
    /*CY_ASSERT(eResult == CY_SYSCLK_SUCCESS); fatal error*/
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      eResult = Cy_SysClk_FllConfigureStandard(tsFllConfig->u32InClkFreq, 
                                               tsFllConfig->u32OutClkFreq, 
                                               CY_SYSCLK_FLLPLL_OUTPUT_AUTO);
      if (CY_SYSCLK_SUCCESS == eResult)
      {
        eResult = Cy_SysClk_FllEnable(WAIT_FOR_STABILIZATION);
        if (CY_SYSCLK_SUCCESS == eResult)
        {
          SRSS->unCLK_FLL_STATUS.stcField.u1UNLOCK_OCCURRED = 1ul;
          /* Set the FLL bypass mode to 3 */
          SRSS->unCLK_FLL_CONFIG3.stcField.u2BYPASS_SEL = (uint32_t)CY_SYSCLK_FLLPLL_OUTPUT_OUTPUT;
          u8HalResult = DRIVER_TRUE;
        }
        else
        {
          u8HalResult = DRIVER_TIMEOUT;
        }
      }
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

/*ECO Enable/Disable*/
uint8_t HAL_Clock_High_External_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
  cy_en_sysclk_status_t eResult = CY_SYSCLK_INVALID_STATE;
  uint8_t u8HalResult = DRIVER_FALSE;
  
  if (FUNC_ENABLE == u8Enable)
  {
    eResult = Cy_SysClk_EcoConfigure(CLK_FREQ_ECO,
                                     LOAD_CAP_IN_PF,
                                     ESR_IN_OHME,
                                     MAX_DRIVE_LEVEL_IN_UW);
    
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      eResult = Cy_SysClk_EcoEnable(u32Timeout);
      /*CY_ASSERT(eResult == CY_SYSCLK_SUCCESS);*/
      if (CY_SYSCLK_SUCCESS == eResult)
      {
        u8HalResult = DRIVER_TRUE;
      }
      else
      {
        u8HalResult = DRIVER_TIMEOUT;
      }
    }
    else
    {
      u8HalResult = DRIVER_PARAMETER_ERROR;
    }
  }
  else
  {
    Cy_SysClk_EcoDisable();
    u8HalResult = DRIVER_TRUE;
  }
  return u8HalResult;
}

/*WCO Enable/Disable*/
uint8_t HAL_Clock_Low_External_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
  cy_en_sysclk_status_t eResult = CY_SYSCLK_INVALID_STATE;
  uint8_t u8HalResult = DRIVER_FALSE;
  
  if (FUNC_ENABLE == u8Enable)
  {
    eResult = Cy_SysClk_WcoEnable(u32Timeout);
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_TIMEOUT;
    }
  }
  else
  {
    Cy_SysClk_WcoDisable();
    u8HalResult = DRIVER_TRUE;
  }
  return u8HalResult;
}

/*IMO Enable/Disable*/
uint8_t HAL_Clock_High_Internal_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
  uint8_t u8HalResult = DRIVER_PARAMETER_ERROR;
  UNUSED(u8Enable);
  UNUSED(u32Timeout);
  /*
  Disabling internal clock sources that are not generating
  a system clock. All clock sources are initially disabled,
  except the IMO. Note that some clock sources, such as
  the crystal oscillators (WCO and ECO) have relatively
  long startup times. Switching these circuits off and on
  may result in more overall current if the system must idle
  while they start up.
  */
  /*Currently SDL 6.5.0 does not implement IMO enable/disable*/
  return u8HalResult;
}

uint8_t HAL_Clock_Low_Internal_Clock_0_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
  cy_en_sysclk_status_t eResult = CY_SYSCLK_INVALID_STATE;
  uint8_t u8HalResult = DRIVER_FALSE;
  Cy_WDT_Unlock();
  if (FUNC_ENABLE == u8Enable)
  {
    eResult = Cy_SysClk_Ilo0Enable();
    
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_TIMEOUT;
    }
  }
  else
  {
    eResult = Cy_SysClk_Ilo0Disable();
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_TIMEOUT;
    }
  }
  Cy_WDT_Lock();
  return u8HalResult;
}

uint8_t HAL_Clock_Low_Internal_Clock_1_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
   cy_en_sysclk_status_t eResult = CY_SYSCLK_INVALID_STATE;
  uint8_t u8HalResult = DRIVER_FALSE;
  Cy_WDT_Unlock();
  if (FUNC_ENABLE == u8Enable)
  {
    Cy_SysClk_Ilo1Enable();
    u8HalResult = DRIVER_TRUE; 
  }
  else
  {
    eResult = Cy_SysClk_Ilo1Disable();
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_TIMEOUT;
    }
  }
  Cy_WDT_Lock();
  return u8HalResult;
}

uint8_t HAL_Clock_PLL_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
  cy_en_sysclk_status_t eResult = CY_SYSCLK_INVALID_STATE;
  uint8_t u8HalResult = DRIVER_FALSE;
  if (FUNC_ENABLE == u8Enable)
  {
    eResult =  Cy_SysClk_PllEnable(PLL_PATH_NO, u32Timeout);
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_TIMEOUT;
    }
  }
  else
  {
    Cy_SysClk_PllDisable(PLL_PATH_NO);
    u8HalResult = DRIVER_TRUE;
  }
  return u8HalResult;
}

uint8_t HAL_Clock_FLL_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
  cy_en_sysclk_status_t eResult = CY_SYSCLK_INVALID_STATE;
  uint8_t u8HalResult = DRIVER_FALSE;
  if (FUNC_ENABLE == u8Enable)
  {
    eResult = Cy_SysClk_FllEnable(u32Timeout);
    if (CY_SYSCLK_SUCCESS == eResult)
    {
      SRSS->unCLK_FLL_STATUS.stcField.u1UNLOCK_OCCURRED = 1ul;
      /* Set the FLL bypass mode to 3 */
      SRSS->unCLK_FLL_CONFIG3.stcField.u2BYPASS_SEL = (uint32_t)CY_SYSCLK_FLLPLL_OUTPUT_OUTPUT;
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_TIMEOUT;
    }
  }
  else
  {
    Cy_SysClk_FllDisable();
    u8HalResult = DRIVER_TRUE;
  }
  return u8HalResult; 
}

uint8_t HAL_Clock_Path_Setting(uint8_t u8ClockSource, uint8_t u8ClockTarget)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  
  if (BYPASSED_PATH_NO > u8ClockSource || u8ClockTarget > CLOCK_HF1)
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  else
  {
    Cy_SysClk_HfClkEnable((cy_en_hfclk_t)u8ClockTarget);
    Cy_SysClk_HfClockSetSource((cy_en_hfclk_t)u8ClockTarget, (cy_en_hf_clk_sources_t)u8ClockSource);
    u8HalResult = DRIVER_TRUE;
  }
  return u8HalResult; 
}

/*** End of file ***/
