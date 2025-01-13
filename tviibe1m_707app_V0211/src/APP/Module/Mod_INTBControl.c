/**
* @file Mod_INTBControl.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-26
* 
* @copyright Copyright (c) 2020
* 
*/

/* include global */
#include "gPinDef.h"

/* include module */
#include "Mod_INTBControl.h"
#include "Mod_DataManagement.h"

/* Include App */
#include "GpioApp.h"
#include "TimerApp.h"

MINTBC_INTBFLAG_CTRL MINTBC_INTBFlag_Ctrl;

static StrategyState_E StrategyState_e = Wait10ms;
static Global_LockState_E MINTBC_u8LockPinStatus = eLocked;
static Global_PowerState_E gPowerState_e = ePowerState_Stop;

void MINTBC_INTBStrategy_Ctrl(void* context)
{
    UNUSED (context);
    
    /* Get Power status */
    gPowerState_e = MMIM_PowerState_Ctrl(ePowerstate_Get);
    
    /* INTB can't go high when power 3.3V is not ready */
    if((gPowerState_e != ePowerState_Stop) &&
       (gPowerState_e != ePowerState_HostShutdown) &&
           (gPowerState_e != ePowerState_AbnormalRun) &&
               (gPowerState_e != ePowerState_AbnormalRun_3V3) &&
                   (gPowerState_e != ePowerState_AbnormalRun_5V0))
    {
        (void)HAL_GPIO_PinWrite(PIN_984_INTB_IN,PIN_HIGH);     // PIN_984_INTB_IN =1
        (void)HAL_GPIO_PinWrite(PIN_MCU_INT,PIN_HIGH);        //  PIN_MCU_INT =1
    }
    
    /*if Fault flag generated from interrupt */
    if((MINTBC_INTBFlag_Ctrl(GETFLAG) == ENABLE) && (MINTBC_u8LockPinStatus == eLocked))    
    {
        switch(StrategyState_e)
        {
        case Wait10ms:
            
            StrategyState_e = AssertINBIN;
            break;
            
        case AssertINBIN:
            /*made INB Falling*/
            (void)HAL_GPIO_PinToggle(PIN_984_INTB_IN); 
            (void)HAL_GPIO_PinToggle(PIN_MCU_INT);
            
            MINTBC_INTBFlag_Ctrl(DISABLE);
            
            StrategyState_e = Wait10ms;
            break;
            
        default:
            
            StrategyState_e = Wait10ms;
            break;
        }
    }
}
Global_LockState_E MINTBC_LockPinStatus_Ctrl(Global_LockState_E u8Status)
{
    switch (u8Status)
    {
    case GETFLAG:
        break;
        
    default:
        MINTBC_u8LockPinStatus = u8Status;
        break;
    }
    
    return MINTBC_u8LockPinStatus;
}

/* use 2ms timer to implement INTB control */
void MINTBC_2msTimerProcess_Install(MINTBC_INTBFLAG_CTRL pINTB_Ctrl_fun)
{
    MINTBC_INTBFlag_Ctrl = pINTB_Ctrl_fun;
    
    /* Create INTB Timer for 2ms */
    HAL_Timer_Init(MINTBC_TIMER_1MINSEC,&gts_2mSec_Timer_Continuous_Config);  //2ms
    
    /*Timer register callback*/
    HAL_Timer_Callback_Register (MINTBC_TIMER_1MINSEC,MINTBC_INTBStrategy_Ctrl,NULL);
    
    /*Timer Enable*/
    HAL_Timer_Active(MINTBC_TIMER_1MINSEC  ,FUNC_ENABLE);
    
    /*INTB_TIMER & COUNTER_FLAG_TIMER timer start*/ 
    HAL_Timer_Start(MINTBC_TIMER_1MINSEC   ,FUNC_ENABLE);
    
    /*Enable INTB_TIMER interrupt & recounting */
    HAL_Timer_Interrupt_Set (MINTBC_TIMER_1MINSEC, FUNC_ENABLE, CYHAL_TCPWM_IRQ_PRIORITY);  
}
