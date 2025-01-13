/***************************************************************************
*\file cyhal_hwmgr.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "cyhal_hwmgr.h"

#define RESOURCE_FREE          (0U)
#define RESOURCE_USING         (1U)
#define RESOURCE_INVALID       (0xFFU)

#define CY_INIT_SCB(x) { \
                      .pBase   = SCB ##x,             \
                      .eClkSrc = PCLK_SCB ##x ## _CLOCK,         \
                      .eIntr   = scb_ ##x ##_interrupt_IRQn,\
                      .pI2CContext  = NULL, \
                      .pUartContext = NULL, \
                      .pSPIContext  = NULL, \
                      .u8SCBRole    = 0U,   \
                      }\

#define UNDEF_SCB(x) { \
                      .pBase   = NULL,             \
                      .eClkSrc = PCLK_CPUSS_CLOCK_TRACE_IN,         \
                      .eIntr   = disconnected_IRQn,\
                      .pI2CContext  = NULL, \
                      .pUartContext = NULL, \
                      .pSPIContext  = NULL, \
                      .u8SCBRole    = 0U,   \
                      }\
                        
#ifdef CYT2B75CAE                         
static cy_scb_t tsSCB [CYHAL_NUM_OF_SCB] =
{
  CY_INIT_SCB(0),CY_INIT_SCB(1),CY_INIT_SCB(2),CY_INIT_SCB(3),
  CY_INIT_SCB(4),CY_INIT_SCB(5),CY_INIT_SCB(6),CY_INIT_SCB(7)  
};
#elif defined (CYT2B65BAE)
static cy_scb_t tsSCB [CYHAL_NUM_OF_SCB] =
{
  CY_INIT_SCB(0),CY_INIT_SCB(1),UNDEF_SCB(2),CY_INIT_SCB(3),
  CY_INIT_SCB(4),CY_INIT_SCB(5),UNDEF_SCB(6),CY_INIT_SCB(7)  
};
#else
    #error Undefined part number
#endif

#define INIT_TCPWM(x) { \
                      .pBase   = TCPWM0_GRP0_CNT ##x,             \
                      .eClkSrc = PCLK_TCPWM0_CLOCKS ##x ,         \
                      .eIntr   = tcpwm_0_interrupts_ ##x ## _IRQn,\
                      .pCallback = NULL,                          \
                      .pContext  = NULL,                          \
                      .u8IsISREnable = NULL,                      \
                      }\

#define UNDEF_TCPWM(x) { \
                      .pBase   = NULL,                             \
                      .eClkSrc = PCLK_CPUSS_CLOCK_TRACE_IN,        \
                      .eIntr   = disconnected_IRQn,                \
                      .pCallback = NULL,                           \
                      .pContext  = NULL,                           \
                      .u8IsISREnable = NULL,                      \
                      }\

/*
  hal_timer_pwm_t use INIT_TCPWM (#define) to expand data struct, 
  U cannot be added after the constent number.
  For example : INIT_TCPWM(0U) not allow
*/
#ifdef tviibh8m
static cy_tcpwm_t tsTCPWM[] = 
{
  INIT_TCPWM(0)  ,INIT_TCPWM(1)  ,INIT_TCPWM(2)
};
#else
static cy_tcpwm_t tsTCPWM [] =  
{
  INIT_TCPWM(0)  ,INIT_TCPWM(1)  ,INIT_TCPWM(2)  ,UNDEF_TCPWM(3) ,
  INIT_TCPWM(4)  ,INIT_TCPWM(5)  ,INIT_TCPWM(6)  ,INIT_TCPWM(7)  ,
  UNDEF_TCPWM(8) ,INIT_TCPWM(9)  ,INIT_TCPWM(10) ,INIT_TCPWM(11) ,
  INIT_TCPWM(12) ,INIT_TCPWM(13) ,INIT_TCPWM(14) ,INIT_TCPWM(15) ,
  INIT_TCPWM(16) ,INIT_TCPWM(17) ,INIT_TCPWM(18) ,INIT_TCPWM(19) ,
  INIT_TCPWM(20) ,INIT_TCPWM(21) ,INIT_TCPWM(22) ,INIT_TCPWM(23) ,
  INIT_TCPWM(24) ,INIT_TCPWM(25) ,INIT_TCPWM(26) ,UNDEF_TCPWM(27),
  UNDEF_TCPWM(28),UNDEF_TCPWM(29),UNDEF_TCPWM(30),UNDEF_TCPWM(31),
  UNDEF_TCPWM(32),INIT_TCPWM(33) ,INIT_TCPWM(34) ,UNDEF_TCPWM(35),
  INIT_TCPWM(36) ,INIT_TCPWM(37) ,INIT_TCPWM(38) ,INIT_TCPWM(39) ,
  INIT_TCPWM(40) ,INIT_TCPWM(41) ,INIT_TCPWM(42) ,UNDEF_TCPWM(43),  
  INIT_TCPWM(44) ,INIT_TCPWM(45) ,INIT_TCPWM(46) ,INIT_TCPWM(47),
  INIT_TCPWM(48) ,INIT_TCPWM(49) ,INIT_TCPWM(50) ,INIT_TCPWM(51),
  INIT_TCPWM(52) ,INIT_TCPWM(53) ,INIT_TCPWM(54) ,INIT_TCPWM(55)
};
#endif

typedef struct {
  uint8_t u8Num;
  uint8_t u8Using;
}divider_24_5;

static void HAL_TCPWM_IRQ(void);
static void HAL_SCB_IRQ(void);
static void HAL_UNDEF_TCPWM_IRQ(void);
/**
	CYT2B6 Divider

	Divider  | Count | Description
	=====================================================================================
	div_24_5 | 8     | Fractional divider, 24.5 bits (24integer bits, 5 fractional bits)
*/
static divider_24_5 tsDivider_24_5 [CYHAL_NUM_OF_24_5_DIV] = 
{
  {0U,RESOURCE_FREE},{1U,RESOURCE_FREE},{2U,RESOURCE_FREE},{3U,RESOURCE_FREE},
  {4U,RESOURCE_FREE},{5U,RESOURCE_FREE},{6U,RESOURCE_FREE},{7U,RESOURCE_USING},
};

cy_scb_t* CYHAL_SCB_Get( uint8_t u8SCB_Num)
{
  cy_scb_t* pscb = NULL;
  if ( u8SCB_Num >= CYHAL_NUM_OF_SCB )
  {
    pscb = NULL;
  }
  else
  {
    pscb = &tsSCB[u8SCB_Num];
    if (NULL == pscb->pBase)
      pscb = NULL;
  }
  return pscb;
}

cy_tcpwm_t* CYHAL_TCPWM_Get( uint8_t u8TCPWM_Num)
{
  cy_tcpwm_t* pTCPWM= NULL;
  if (u8TCPWM_Num >= CYHAL_NUM_OF_TCPWM)
  {
    pTCPWM = NULL;
  }
  else
  {
    pTCPWM = &tsTCPWM[u8TCPWM_Num];
    if (NULL == pTCPWM->pBase)
      pTCPWM = NULL;
  }
  return pTCPWM;
}

static uint8_t Divider_24_5_Get(void)
{
  uint8_t u8DivNum = RESOURCE_INVALID;
  
  for (uint8_t i = 0 ; i < CYHAL_NUM_OF_24_5_DIV ; i ++)
  {
    if (RESOURCE_FREE == tsDivider_24_5[i].u8Using)
    {
      tsDivider_24_5[i].u8Using = RESOURCE_USING;
      u8DivNum = tsDivider_24_5[i].u8Num;
      break;
    }
  }
  return u8DivNum;
}

static void SetPeripheFracDiv24_5( uint64_t u64TargetFreq, uint64_t u64SourceFreq, uint8_t u8DivNum)
{
    uint64_t u64Temp = ((uint64_t)u64SourceFreq << 5ull);
    uint32_t u32DivSetting;

    u32DivSetting = (uint32_t)(u64Temp / u64TargetFreq);
    Cy_SysClk_PeriphSetFracDivider(CY_SYSCLK_DIV_24_5_BIT, u8DivNum, 
                                   (((u32DivSetting >> 5u) & 0x00000FFFul) - 1ul), 
                                   (u32DivSetting & 0x0000001Ful));
}

uint8_t CYHAL_SCB_Divider24_5_Assign( const cy_scb_t* ptsSCB,uint64_t u64TargetFreq, uint64_t u64SourceFreq)
{
  uint8_t u8Result = DRIVER_FALSE;
  uint8_t u8DivNum = Divider_24_5_Get();
  /*---------------------*/
  /* Clock Configuration */
  /*---------------------*/
  if (RESOURCE_INVALID == u8DivNum || NULL == ptsSCB)
  {
    u8Result = DRIVER_FALSE;
  }
  else
  {
    Cy_SysClk_PeriphAssignDivider(ptsSCB->eClkSrc, CY_SYSCLK_DIV_24_5_BIT, u8DivNum);
    SetPeripheFracDiv24_5(u64TargetFreq, u64SourceFreq, u8DivNum);
    Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_24_5_BIT, u8DivNum);
    u8Result = DRIVER_TRUE;
  }
  return u8Result;
}

uint8_t CYHAL_SCB_Clock_Init(cy_scb_t* pSCB ,uint32_t u32TargetFreq)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  uint64_t u64PeriFreq = 0U;
 
  if (CY_SYSCLK_SUCCESS == Cy_SysClk_GetClkPeriFrequency((uint32_t*)&u64PeriFreq))
  {
    u8HalResult = CYHAL_SCB_Divider24_5_Assign(pSCB,u32TargetFreq, u64PeriFreq);
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t CYHAL_TCPWM_IRQ_Callback_Set(uint8_t u8TCPWM_Num, CYHAL_TCPWM_ISR pCallback, void* pContext)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pTcpwm = CYHAL_TCPWM_Get(u8TCPWM_Num);
   
  cy_stc_sysint_irq_t tsIrq_cfg = 
  {
    .sysIntSrc  = tcpwm_0_interrupts_0_IRQn, 
    .intIdx     = CYHAL_TCPWM_IRQn,
    .isEnabled  = true,
  };

  if ( (NULL != pTcpwm) && (NULL != pCallback) )
  {
    tsIrq_cfg.sysIntSrc = pTcpwm->eIntr;
    
    pTcpwm->pCallback = pCallback;
    pTcpwm->pContext  = pContext;
    pTcpwm->u8IsISREnable = FUNC_ENABLE;
    
    Cy_SysInt_InitIRQ(&tsIrq_cfg);
    Cy_SysInt_SetSystemIrqVector(tsIrq_cfg.sysIntSrc, HAL_TCPWM_IRQ);
  
    /* Set the Interrupt Priority & Enable the Interrupt */
    NVIC_SetPriority(CYHAL_TCPWM_IRQn, CYHAL_TCPWM_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(CYHAL_TCPWM_IRQn);
    NVIC_EnableIRQ(CYHAL_TCPWM_IRQn);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t CYHAL_SCB_IRQ_Set(cy_scb_t* pSCB,uint8_t u8SCBRole)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  
  cy_stc_sysint_irq_t tsIrq_cfg = 
  {
    .sysIntSrc  = scb_0_interrupt_IRQn, 
    .intIdx     = CYHAL_SCB_IRQn,
    .isEnabled  = true,
  };

  if ( (NULL != pSCB) && 
       ((CYSCB_ROLE_UART == u8SCBRole) || 
        (CYSCB_ROLE_I2C  == u8SCBRole) ||
        (CYSCB_ROLE_SPI  == u8SCBRole))
     )
  {
    pSCB->u8SCBRole = u8SCBRole;
    tsIrq_cfg.sysIntSrc = pSCB->eIntr;
     
    Cy_SysInt_InitIRQ(&tsIrq_cfg);
    Cy_SysInt_SetSystemIrqVector(tsIrq_cfg.sysIntSrc, HAL_SCB_IRQ);
  
    /* Set the Interrupt Priority & Enable the Interrupt */
    NVIC_SetPriority(CYHAL_SCB_IRQn, CYHAL_SCB_IRQ_PRIORITY);
    NVIC_ClearPendingIRQ(CYHAL_SCB_IRQn);
    NVIC_EnableIRQ(CYHAL_SCB_IRQn);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

/**
 * TCPWM (Countr / PWM) IRQ Handler
 */
#define TCPWM_ISR_CASE(x) case tcpwm_0_interrupts_ ##x ## _IRQn: pTcpwm = CYHAL_TCPWM_Get(x); break
static void HAL_UNDEF_TCPWM_IRQ(void)
{
 ;
}

static void HAL_TCPWM_IRQ(void)
{
  cy_tcpwm_t* pTcpwm = NULL;
  cy_en_intr_t eInterrupt_src;
  eInterrupt_src = Cy_SysInt_GetIntStatus(CYHAL_TCPWM_IRQn);
  
  switch(eInterrupt_src)
  {
    TCPWM_ISR_CASE(0); 
    TCPWM_ISR_CASE(1); 
    TCPWM_ISR_CASE(2);
    TCPWM_ISR_CASE(4); 
    TCPWM_ISR_CASE(5); 
    TCPWM_ISR_CASE(6); 
    TCPWM_ISR_CASE(7); 
    TCPWM_ISR_CASE(9); 
    TCPWM_ISR_CASE(10);
    TCPWM_ISR_CASE(11);
    TCPWM_ISR_CASE(12);
    TCPWM_ISR_CASE(13);
    TCPWM_ISR_CASE(14);
    TCPWM_ISR_CASE(15);
    TCPWM_ISR_CASE(16);
    TCPWM_ISR_CASE(17);
    TCPWM_ISR_CASE(18);
    TCPWM_ISR_CASE(19);
    TCPWM_ISR_CASE(20);
    TCPWM_ISR_CASE(21);
    TCPWM_ISR_CASE(22);
    TCPWM_ISR_CASE(23);
    TCPWM_ISR_CASE(24);
    TCPWM_ISR_CASE(25);
    TCPWM_ISR_CASE(26);
    TCPWM_ISR_CASE(33);
    TCPWM_ISR_CASE(34);
    TCPWM_ISR_CASE(36);
    TCPWM_ISR_CASE(37);
    TCPWM_ISR_CASE(38);
    TCPWM_ISR_CASE(39);
    TCPWM_ISR_CASE(40);
    TCPWM_ISR_CASE(41);
    TCPWM_ISR_CASE(42);
    TCPWM_ISR_CASE(44);
    TCPWM_ISR_CASE(45);
    TCPWM_ISR_CASE(46);
    TCPWM_ISR_CASE(47);
    TCPWM_ISR_CASE(48);
    TCPWM_ISR_CASE(49);
    TCPWM_ISR_CASE(50);
    TCPWM_ISR_CASE(51);
    TCPWM_ISR_CASE(52);
    TCPWM_ISR_CASE(53);
    TCPWM_ISR_CASE(54);
    TCPWM_ISR_CASE(55);
 
    default:
      pTcpwm = NULL;
      break;
  }
  if (NULL != pTcpwm)
  {
    if (FUNC_ENABLE == pTcpwm->u8IsISREnable)
    {
      
      if(Cy_Tcpwm_Counter_GetCC0_IntrMasked(pTcpwm->pBase))
      {
        /* ISR PWM Callback function*/
        (*pTcpwm->pCallback) (pTcpwm->pContext);
        Cy_Tcpwm_Counter_ClearCC0_Intr(pTcpwm->pBase);
      }
      else
      if(Cy_Tcpwm_Counter_GetTC_IntrMasked(pTcpwm->pBase) )
      {
        /* ISR TIMER Callback function*/
        (*pTcpwm->pCallback) (pTcpwm->pContext);
        Cy_Tcpwm_Counter_ClearTC_Intr(pTcpwm->pBase);
      }
      else
        HAL_UNDEF_TCPWM_IRQ();
    }
  }
}

/**
 * SCB (I2C Slave / UART) IRQ Handler
 */
#define SCB_ISR_CASE(x) case scb_ ##x ##_interrupt_IRQn: pSCB = CYHAL_SCB_Get(x); break

static void HAL_UNDEF_SCB_IRQ(void)
{
 ;
}

static void HAL_SCB_IRQ(void)
{
  cy_scb_t* pSCB = NULL;
  cy_en_intr_t eInterrupt_src;
  eInterrupt_src = Cy_SysInt_GetIntStatus(CYHAL_SCB_IRQn);
  switch(eInterrupt_src)
  {
    SCB_ISR_CASE(0);
    SCB_ISR_CASE(1);
    SCB_ISR_CASE(3);
    SCB_ISR_CASE(4);
    SCB_ISR_CASE(5);
    SCB_ISR_CASE(7);
#ifdef CYT2B75CAE 
    SCB_ISR_CASE(2);
    SCB_ISR_CASE(6);
#endif
    default:
      pSCB = NULL;
      break;
  }
  
  if (NULL != pSCB)
  {
    switch (pSCB->u8SCBRole)
    {
      case CYSCB_ROLE_UART:
        /* UART interrupt handler */
        Cy_SCB_UART_Interrupt(pSCB->pBase,pSCB->pUartContext);
        break;
      case CYSCB_ROLE_I2C:
        /* I2C interrupt handler */
        Cy_SCB_I2C_Interrupt(pSCB->pBase, pSCB->pI2CContext);
        break;
      case CYSCB_ROLE_SPI:
        /* SPI interrupt handler*/
        Cy_SCB_SPI_Interrupt(pSCB->pBase, pSCB->pSPIContext);
        break;
      default:
        HAL_UNDEF_SCB_IRQ();
        break;
    }
  }
}
/*** End of file ***/

