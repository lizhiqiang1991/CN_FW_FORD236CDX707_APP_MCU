/***************************************************************************
*\file hal_sp.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_sp_def.h"
#include "hal_sp.h"

#define HAL_STACK_OVERFLOW       (0xffU)
#define HAL_ONE_HUNDRED_PERCENT  (100U)

#if defined(__GNUC__)
  
  extern void * __stack_start__; /* - Linker created - */
  extern void * __stack_end__; 
  extern void * __stack_size__;
  
#elif defined ( __ICCARM__ )
  
  extern void * CSTACK$$Base;   /* - Linker created - */
  extern void * CSTACK$$Limit;
  
#else
  #warning "hal_sp driver is supported only by GCC and IAR compiler"
#endif

static uint32_t MSP_Get(void)
{
  register uint32_t u32Result;
  __ASM volatile ("MRS %0, msp" : "=r" (u32Result) );
  return(u32Result);
}
 
Status_Return_Typedef_e HAL_SP_Init(Sp_Config_Typedef_s *tsSp)
{
  Status_Return_Typedef_e eResult = ePROCESS_FAIL;
  if (NULL != tsSp)
  {
    #if defined(__GNUC__)
      
      (tsSp->pvStackStart)    = &__stack_start__;
      (tsSp->pvMinStackSize)  = &__stack_size__;
      (tsSp->pvStackEnd)      = &__stack_end__;
      
    #elif defined ( __ICCARM__ )
    
      (tsSp->pvStackStart)    = &CSTACK$$Base;
      (tsSp->pvStackEnd)      = &CSTACK$$Limit;
      (tsSp->pvMinStackSize)  = (void*) ( ((uint32_t)(tsSp->pvStackEnd)) - ((uint32_t)(tsSp->pvStackStart)) );
    
    #else
      
      (tsSp->pvStackStart)    = (void*)NULL;
      (tsSp->pvMinStackSize)  = (void*)NULL;
      (tsSp->pvStackEnd)      = (void*)NULL;
    
    #endif

    eResult = ePROCESS_OK;
  }
  else
  {
    eResult = ePROCESS_FAIL;
  }
  return eResult;
}

Status_Return_Typedef_e HAL_SP_Overflow_Check(Sp_Config_Typedef_s *tsSp)
{
  Status_Return_Typedef_e eResult = ePROCESS_FAIL;
  uint8_t u8Rate = HAL_SP_Usage_Get(tsSp);
  if (HAL_STACK_OVERFLOW == u8Rate)
  {
    eResult = ePROCESS_FAIL;
  }
  else
  {
    eResult = ePROCESS_OK;
  }
  return eResult;
}

uint8_t HAL_SP_Usage_Get(Sp_Config_Typedef_s *tsSp)
{
  uint8_t u8Percent = HAL_STACK_OVERFLOW;

  UNUSED(tsSp->pvStackStart);

  if (NULL != tsSp)
  {
    uint32_t u32CurSp      = MSP_Get(); /* uint32_t __get_MSP(void) Return current value of the Main Stack Pointer (MSP)*/
    uint32_t u32StackSize  = (uint32_t) (tsSp->pvMinStackSize);
    uint32_t u32StackEnd   = (uint32_t) (tsSp->pvStackEnd);
    uint32_t u32UseBytes = u32StackEnd - u32CurSp;

    if (u32UseBytes >= u32StackSize)
      u8Percent = HAL_STACK_OVERFLOW;
    else
    {
      u8Percent = (uint8_t)((float) ( (float)u32UseBytes/(float)u32StackSize ) * HAL_ONE_HUNDRED_PERCENT);
    }
  }
  else
  {
    u8Percent = HAL_STACK_OVERFLOW;
  }
  return u8Percent;
}

/*** End of file ***/

  