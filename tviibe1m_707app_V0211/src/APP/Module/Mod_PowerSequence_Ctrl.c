/*
* Mod_PowerSequence_Ctrl.c
*
*  Created on: 2020�~11��10��
*      Author: DerekWen
*/

/* include global */
#include "gPinDef.h"
#include "Ford_Config.h"

/* include module */
#include "Mod_PowerSequence_Ctrl.h"
#include "Mod_DataManagement.h"
#include "Mod_BLBrightness_Ctrl.h"
#include "Mod_BLEN_Ctrl.h"
#include "Mod_DisplayEN_Ctrl.h"
#include "Mod_DetectI2C.h"

/* include APP */
#include "GpioApp.h"
#include "TimerApp.h"
#include "I2CSlaveApp.h"
#include "I2CMasterApp.h"
#include "PWMApp.h"
#include "FlashApp.h"

uint8_t u8WatchDogToogle_EN = DISABLE;


static MPSC_PowerEvent_E MPSC_ePowerEvent = ePowerOFF;
static Global_PowerState_E MPSC_ePowerState = ePowerState_Stop;


/**
* @brief WD feed in 10ms timer IRQ
* 
* @return uint8_t 
*/
void MPSC_WatchDog_Feed(void* context)
{
    UNUSED (context);
    
    /* count for 10ms , period 20ms */
    if(u8WatchDogToogle_EN == ENABLE)
    {
        (void)HAL_GPIO_PinToggle(PIN_MAX20419_XWDI); /* feed   MAX20419 WD */
    }
    else
    {
        ;/* code */
    }
}

void MPSC_Module_Inital(void)
{
    /* Create WDT Timer for 10ms */
    (void)HAL_Timer_Init(WDT_TIMER_10MINISEC,&gts_10mSec_Timer_Continuous_Config);  //10ms
    
    /*Timer register callback*/
    (void)HAL_Timer_Callback_Register (WDT_TIMER_10MINISEC,MPSC_WatchDog_Feed,NULL);
    
    /*Timer Enable*/
    (void)HAL_Timer_Active(WDT_TIMER_10MINISEC  ,FUNC_ENABLE);
    
    /*LED_TIMER & COUNTER_FLAG_TIMER timer start*/ 
    (void)HAL_Timer_Start(WDT_TIMER_10MINISEC   ,FUNC_ENABLE);
    
    /*Enable LED_TIMER interrupt & recounting */
    (void)HAL_Timer_Interrupt_Set (WDT_TIMER_10MINISEC, FUNC_ENABLE, CYHAL_TCPWM_IRQ_PRIORITY);    
}

uint8_t MPSC_Check_Power_Alive(uint8_t status, uint8_t times)
{
    uint8_t isAlive = NUM_ZERO;
    
    if (times == NUM_ZERO)
    {   /* times can not be zero */
        return false;
    }
    
    for( ; times>(uint8_t)NUM_ZERO; times--)
    {
        switch(status)
        {
        case PIN_LM61460_PG:
            (void)HAL_GPIO_PinRead(PIN_LM61460_PG,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_PGOOD_LM61460; */
            break;
        case PIN_MAX20419_PG:
            (void)HAL_GPIO_PinRead(PIN_MAX20419_PG,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_PGOOD_MAX20419; */
            break;
        case PIN_MAX25221_FLTB:
            (void)HAL_GPIO_PinRead(PIN_MAX25221_FLTB,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_PGOOD_MAX25221_FLTB; */
            break;
        case PIN_PANEL_ABD:
            (void)HAL_GPIO_PinRead(PIN_PANEL_ABD,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_PIN_PANEL_ABD; */
            break;
        case PIN_984_LOCK:
            (void)HAL_GPIO_PinRead(PIN_984_LOCK,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_PIN_984_LOCK; */
            break;
        case PIN_25210_RESET_L:
            (void)HAL_GPIO_PinRead(PIN_25210_RESET_L,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_PIN_25210_RESET_L; */
            break;
        case PIN_25210_RESET_R:
            (void)HAL_GPIO_PinRead(PIN_25210_RESET_R,&isAlive);
            /* ucErrorPower &= ~(uint8_t)ERROR_PIN_25210_RESET_R; */
            break;
        default:
            break;
        }
        
        if (isAlive == NUM_ONE)
        {   /* pin good! */
            return true;
        }
        else
        {
            Cy_SysLib_Delay(1); // 1ms
        }
        
    }
    
    /* error flag */
    MMIM_Check_Power_Error(status);
    return false;
}

Global_PowerState_E MPSC_PowerSequence_ON(void)
{   
    //uint8_t u8Input = 0U;   
    uint8_t MAX25221_SlavAddr = 0x21U;
    uint8_t MAX25221_FLTMask1[2] = {0x03U,0x55U};
    uint8_t MAX25221_FLTMask2[2] = {0x04U,0x73U};
    uint8_t MAX25221_FLTMaskR1[2] = {0x05U,NUM_ZERO};
    uint8_t MAX25221_FLTMaskR2[2] = {0x06U,NUM_ZERO};

    /* ----- LM61460 -----*/
    (void)HAL_GPIO_PinWrite(PIN_LM61460_EN,PIN_HIGH);     // P3V3_EN
    
    
    Cy_SysLib_Delay(7);  // Delay 7 msec
    
    if(MPSC_Check_Power_Alive(PIN_LM61460_PG, 30U) == (uint8_t)NUM_ZERO)
    {
        /*for 3V3 not good*/
        MMIM_Client_Diag_Set(General_Byte_2 , (MMIM_Client_Diag_Get(General_Byte_2) | P3V3_PO_Fail));
    }
    else
    {
        MMIM_Client_Diag_Set(General_Byte_2 , (MMIM_Client_Diag_Get(General_Byte_2) & (~P3V3_PO_Fail)));
    }
    
    /* ----- MAX20419 -----*/
    (void)HAL_GPIO_PinWrite(PIN_20419_EN1,PIN_HIGH);    // P5V_EN
    (void)HAL_GPIO_PinWrite(PIN_20419_EN2,PIN_HIGH);    // P1V8_EN
    (void)HAL_GPIO_PinWrite(PIN_20419_EN3,PIN_HIGH);    // P1V1_EN
    
    
    Cy_SysLib_Delay(20);  // Delay 20 msec,for 5V,1V8,1V1 out
    
    
    // close detect 20416 pg pin for shorten boot time
    /*
    if(MPSC_Check_Power_Alive(PIN_20419_RESET1, 30U) == (uint8_t)NUM_ZERO)
    {
    ; //for 5V not good
}
    else
    {
    ;
}
    
    if(MPSC_Check_Power_Alive(PIN_20419_RESET2, 30U) == (uint8_t)NUM_ZERO)
    {
    ; //for 1V8 not good
}
    else
    {
    ;
}
    
    if(MPSC_Check_Power_Alive(PIN_20419_RESET3, 30U) == (uint8_t)NUM_ZERO)
    {
    ; //for 1V1 not good
}
    else
    {
    ;
}
    */
    
    Cy_SysLib_Delay(2);  // Delay 2 msec
    
    /* ---I2C initial ----*/
    i2cMessagesInit();
    I2CS_Initialze();
    I2CM_Initialze();
    
    /* ----- DS90UH984 -----*/
    (void)HAL_GPIO_PinWrite(PIN_984_PDB,PIN_HIGH); 
    
    /* ----- INTB initial set high -----*/
    (void)HAL_GPIO_PinWrite(PIN_984_INTB_IN,PIN_HIGH);     // PIN_984_INTB_IN =1
    (void)HAL_GPIO_PinWrite(PIN_MCU_INT,PIN_HIGH);        //  PIN_MCU_INT =1
    
    /* ----- MAX20419 -----*/
    //(void)HAL_GPIO_PinWrite(PIN_MAX20419_XWDI,PIN_LOW);    
    //u8WatchDogToogle_EN = ENABLE;  
    
    Cy_SysLib_Delay(10);  // Delay 10 msec
    
    /* ----- MAX25221 -----*/
    (void)HAL_GPIO_PinWrite(PIN_Panel_XON_MCU,PIN_HIGH);
    (void)HAL_GPIO_PinWrite(PIN_MAX25221_EN,PIN_HIGH); // MAX25221_EN
    
    // /* ----- MAX25210 -----*/
    // (void)HAL_GPIO_PinWrite(PIN_25210_EN_L,PIN_HIGH);  // HV LDO BL on
    // (void)HAL_GPIO_PinWrite(PIN_25210_EN_R,PIN_HIGH);  // HV LDO BL on

    /* ----- MAX25210  -----*/
    //(void)HAL_GPIO_PinRead(PIN_25210_RESET_L,&u8Input);
    if(MPSC_Check_Power_Alive(PIN_25210_RESET_L, 30U) == (uint8_t)NUM_ZERO)
    {
        ;/*for MAX25210  BL LDO  not good*/
    }
    else
    {
        ;
    }
    // (void)HAL_GPIO_PinRead(PIN_25210_RESET_R,&u8Input);
    if(MPSC_Check_Power_Alive(PIN_25210_RESET_R, 30U) == (uint8_t)NUM_ZERO)
    {
        ;/*for MAX25210  BL LDO  not good*/
    }
    else
    {
        ;
    }

    Cy_SysLib_Delay(60);  // Delay 120 msec -> change to 60 ms

    /* clear initial error flag */
    I2CM_Write(MAX25221_SlavAddr, MAX25221_FLTMask1[0], &MAX25221_FLTMask1[1], sizeof(MAX25221_FLTMask1[1]));
    I2CM_Write(MAX25221_SlavAddr, MAX25221_FLTMask2[0], &MAX25221_FLTMask2[1], sizeof(MAX25221_FLTMask2[1]));
    I2CM_Read(MAX25221_SlavAddr, MAX25221_FLTMaskR1[0], &MAX25221_FLTMaskR1[1], sizeof(MAX25221_FLTMaskR1[1]));
    I2CM_Read(MAX25221_SlavAddr, MAX25221_FLTMaskR2[0], &MAX25221_FLTMaskR2[1], sizeof(MAX25221_FLTMaskR2[1]));
    /* ----- MAX25221_FLTB -----*/
    // (void)HAL_GPIO_PinRead(PIN_MAX25221_FLTB,&u8Input);
    if(MPSC_Check_Power_Alive(PIN_MAX25221_FLTB, 30U) == (uint8_t)NUM_ZERO)
    {
        ;/*for MAX25221_FLTB not good*/
    }
    else
    {
        ;
    }

    Cy_SysLib_Delay(10);  // Delay 10 msec   

    /* ----- MAX25210 -----*/
    (void)HAL_GPIO_PinWrite(PIN_25210_EN_L,PIN_HIGH);  // HV LDO BL on
    (void)HAL_GPIO_PinWrite(PIN_25210_EN_R,PIN_HIGH);  // HV LDO BL on

    Cy_SysLib_Delay(2); 

    /* ----- MAX25240 -----*/
    (void)HAL_GPIO_PinWrite(PIN_25240_EN_L,PIN_HIGH); // Buck-Boost turn on
    (void)HAL_GPIO_PinWrite(PIN_25240_EN_R,PIN_HIGH); // Buck-Boost turn on

    Cy_SysLib_Delay(2); 
	/* ----- HX8880 -----*/
    (void)HAL_GPIO_PinWrite(PIN_TCON_RESET,PIN_HIGH); // TCON_RESET

    Cy_SysLib_Delay(10);  // Delay 10 msec, TCON-Reload
    
    
    (void)MMIM_PwrInitStatus_Ctrl(ENABLE);
    
    if(MMIM_VolRestRqFlag_Ctrl(GETFLAG) == ENABLE)
    {
        /* maintain RST_RQ */
        MMIM_DisplayStatusReg_Ctrl(SOURENOTFROMI2C,eResetRequest,true); 
        (void)MMIM_VolRestRqFlag_Ctrl(DISABLE);     
    }
    
#ifdef AUTORUN
    uint8_t u8Wr_Data[2];
       
    u8Wr_Data[0] = 0U; 
    u8Wr_Data[1] = 0x04;
       
    Cy_SysLib_Delay(50);
    /* panel on */
    (void)HAL_GPIO_PinWrite(PIN_PANEL_RESET,PIN_HIGH);
    
    Cy_SysLib_Delay(50);
    /* backlight on */
    (void)HAL_GPIO_PinWrite(PIN_25240_EN_L,PIN_HIGH); 
    (void)HAL_GPIO_PinWrite(PIN_25240_EN_R,PIN_HIGH); 
    
    Cy_SysLib_Delay(200);
    //I2CM_Write(TCON_ADDRESS_ID, 0x04, &u8Wr_Data[0], sizeof(u8Wr_Data));
       
    //MBLC_TconApp_Backlight_Output(TCON_MAX_DUTY/2);
        
     Cy_SysLib_Delay(50);  
    (void)HAL_GPIO_PinWrite(PIN_TCON_BIST,PIN_HIGH);        /* High = Enable BIST */
    (void)HAL_GPIO_PinWrite(PIN_TCON_BIST_STOP,PIN_HIGH);   /* High = Enable change pattern */
    
    while(1);
#endif    
    
    
    
    // (void)MBLC_TconApp_Backlight_Output(NUM_ZERO);
    //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, 500U);
    return ePowerState_Standby;
}

Global_PowerState_E MPSC_PowerSequence_OFF(Global_PowerState_E ePowerState)
{
    /* ----- MAX25240 -----*/
    (void)HAL_GPIO_PinWrite(PIN_25240_EN_L,PIN_LOW); // Buck-Boost turn off
    (void)HAL_GPIO_PinWrite(PIN_25240_EN_R,PIN_LOW); // Buck-Boost turn off  
    
    /* ----- MAX25210  -----*/
    (void)HAL_GPIO_PinWrite(PIN_25210_EN_L,PIN_LOW);  // HV LDO BL off
    (void)HAL_GPIO_PinWrite(PIN_25210_EN_R,PIN_LOW);  // HV LDO BL off   
    
    /* ----- HX82105 -----*/
    (void)HAL_GPIO_PinWrite(PIN_PANEL_RESET,PIN_LOW); // Panel_RESET  =0
    
    /* ----- HX8880 -----*/
    (void)HAL_GPIO_PinWrite(PIN_TCON_RESET,PIN_LOW); // TCON_RESET = 0
    
    /* ----- MAX25221 -----*/
    (void)HAL_GPIO_PinWrite(PIN_MAX25221_EN,PIN_LOW); // MAX25221_DIS   
    
    /* ----- MAX25210  -----*/
    (void)HAL_GPIO_PinWrite(PIN_25210_EN_L,PIN_LOW);  // HV LDO BL off
    (void)HAL_GPIO_PinWrite(PIN_25210_EN_R,PIN_LOW);  // HV LDO BL off     
    
    
    u8WatchDogToogle_EN = DISABLE;  
    /* ----- MAX20419 -----*/
    (void)HAL_GPIO_PinWrite(PIN_MAX20419_XWDI,PIN_LOW);    
    
    /* ----- DS90UH984 -----*/
    
    Cy_SysLib_Delay(3);  // Delay 3 msec
    
    if(ePowerState != ePowerState_AbnormalRun)
    {
        (void)HAL_GPIO_PinWrite(PIN_984_PDB,PIN_LOW);
    }
    
    (void)HAL_GPIO_PinWrite(PIN_984_INTB_IN,PIN_LOW);     // PIN_984_INTB_IN =0
    (void)HAL_GPIO_PinWrite(PIN_MCU_INT,PIN_LOW);        //  PIN_MCU_INT =0
    
    
    /* ----- MAX20419 ----*/
    (void)HAL_GPIO_PinWrite(PIN_20419_EN1,PIN_HIGH);   // 5V off
    (void)HAL_GPIO_PinWrite(PIN_20419_EN2,PIN_HIGH);   // 1V8 off
    (void)HAL_GPIO_PinWrite(PIN_20419_EN3,PIN_HIGH);   // 1V1 off
    
    
    /* ----- LM61460 -----*/
    (void)HAL_GPIO_PinWrite(PIN_LM61460_EN,PIN_LOW);     // P3V3_DIS
    
    if(MMIM_PwrInitStatus_Ctrl(GETFLAG) == ENABLE)
    {
        I2CM_DeInitialze();
        I2CS_DeInitialze();
    }
    
    
    (void)HAL_GPIO_PinWrite(PIN_PANEL_FPCA_DET_OUT_R,PIN_LOW);  // FPC ouput off
    (void)HAL_GPIO_PinWrite(PIN_PANEL_FPCA_DET_OUT_L,PIN_LOW);  // FPC ouput off
    (void)HAL_GPIO_PinWrite(PIN_TCON_BIST,PIN_LOW);     // PIN_TCON_BIST ,BIST disable
    (void)HAL_GPIO_PinWrite(PIN_TCON_BIST_STOP,PIN_LOW);     // pattern stop
    (void)HAL_GPIO_PinWrite(PIN_MAX20419_XWDI,PIN_LOW);
    
    //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, NUM_ZERO);
    (void)MMIM_PwrInitStatus_Ctrl(DISABLE);
    MMIM_DisplayStatusReg_Ctrl(SOUREFROMI2C,eDisplayInitialized,LEVEL_LOW);
    
    return ePowerState_Stop;
}


Global_PowerState_E MPSC_PowerSequence_Shutdown(Global_LockState_E eLockState)
{
    (void)MBLBC_BLBrightness_Ctrl(NUM_ZERO,NUM_ZERO,DISABLE);/* PWM DIM = 0*/
    
    /* turn off BL */
    (void)MBLC_Display_Ctrl(DISABLE, MPSC_ePowerState, eLockState);  
    
    (void)MMIM_SourceDriver_Ctrl(DISABLE);
    (void)MDEN_DispENProcess_Ctrl(DISABLE,MPSC_ePowerState, eLockState);
    
    (void)MPSC_PowerSequence_OFF(ePowerState_HostShutdown);	
    
    return ePowerState_HostShutdown;
}

Global_PowerState_E MPSC_PowerSequence_abnormalVol (Global_LockState_E eLockState)
{
    Global_PowerState_E ePowerState = ePowerState_Stop;
    
    (void)MBLBC_BLBrightness_Ctrl(NUM_ZERO,NUM_ZERO,DISABLE);/* DIM = 0*/
    /* turn off bl */
    (void)MBLC_Display_Ctrl(DISABLE,MPSC_ePowerState,eLockState);
    
    (void)MMIM_SourceDriver_Ctrl(DISABLE);
    (void)MDEN_DispENProcess_Ctrl(DISABLE,MPSC_ePowerState,eLockState);
    
    ePowerState = ePowerState_AbnormalRun;
    
    /* 20211110 leo, do not shut down at abnomal voltage. */
    //(void)MPSC_PowerSequence_OFF(ePowerState);    
    
    
    
    return ePowerState;
    
}

Global_PowerState_E MPSC_PowerSequence_MAX20419Fault (Global_LockState_E eLockState)
{
    Global_PowerState_E ePowerState = ePowerState_Stop;
    //uint8_t *pErrorCount;
    uint8_t u8ShutDownStatus = 0U;
    //uint8_t u8FlashBuf[128];
    
    u8ShutDownStatus |= MMIM_pI2CMessage_Get()->display_shutdown.SHDWN;
    
    //pErrorCount = (uint8_t*)MMIM_ErrorCountArray_Get();
    
    if(u8ShutDownStatus == 0x01U)
    {   
        /* case I2C */
        (void)MBLBC_BLBrightness_Ctrl(NUM_ZERO,NUM_ZERO,DISABLE);/* PWM DIM = 0*/
        
        /* turn off BL */
        (void)MBLC_Display_Ctrl(DISABLE, MPSC_ePowerState, eLockState);   
        
        (void)MMIM_SourceDriver_Ctrl(DISABLE);
        (void)MDEN_DispENProcess_Ctrl(DISABLE,MPSC_ePowerState, eLockState); 
        
        /* Power Off Sequence */
        (void)MPSC_PowerSequence_OFF(ePowerState); 
        (void)MMIM_VolRestRqFlag_Ctrl(ENABLE);      
        
        ePowerState = ePowerState_HostShutdown;
    }
    else
    {	
        //(void)MBLBC_BLBrightness_Ctrl(NUM_ZERO,NUM_ZERO,DISABLE);/* PWM DIM = 0*/
        
        /* turn off BL */
        //(void)MBLC_Display_Ctrl(DISABLE, MPSC_ePowerState, eLockState); 
        
        //(void)MDEN_DispENProcess_Ctrl(DISABLE,MPSC_ePowerState, eLockState);  
        
        //ePowerState = ePowerState_Stop; 
        (void)MMIM_VolRestRqFlag_Ctrl(ENABLE); 
        MMIM_DisplayStatusReg_Ctrl(SOURENOTFROMI2C,eResetRequest,true); 
        ePowerState = MPSC_PowerSequence_abnormalVol(eLockState);
    }
    
    /*write error state to work flash when battery voltage get low*/
    //Leo modify: write work flash at low battery will cause error.
    //(void)memset(&u8FlashBuf[0], 0xFFU, sizeof(u8FlashBuf));   
    //(void)memcpy(&u8FlashBuf[0], pErrorCount,  ErrorNum);    
    //(void)Flash_Work_Sector_Write(ErrorBackupAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FlashBuf[0U], (sizeof(u8FlashBuf)/sizeof(uint8_t)));
    
    return ePowerState;
}


Global_PowerState_E MPSC_PowerSequence_Ctrl(MPSC_PowerEvent_E ePowerEvent, Global_LockState_E eLockState,Global_PowerState_E ePowerState)
{
    
    MPSC_ePowerEvent = ePowerEvent;
    
    MPSC_ePowerState = ePowerState;
    
    switch (MPSC_ePowerEvent)
    {
    case ePowerOFF:
        
        MPSC_ePowerState = MPSC_PowerSequence_OFF(ePowerState_Stop);
        
        break;
    case ePowerON:
        /* code */
        
        MPSC_ePowerState = MPSC_PowerSequence_ON();
        
        break;
    case ePowerShutdown:
        /* code */
        
        MPSC_ePowerState =  MPSC_PowerSequence_Shutdown(eLockState);
        
        break;  
    case eWakeMAX20419Fault:
        /* code */
        
        MPSC_ePowerState = MPSC_PowerSequence_MAX20419Fault(eLockState);
        
        break;
    case eAbnormalSequence:
        /* code */
        
        MPSC_ePowerState = MPSC_PowerSequence_MAX20419Fault(eLockState);
        
        break;
    case  eSoftwareReset:   /*TBD */
        
        Cy_SysLib_Delay(25);  
        
        NVIC_SystemReset(); // Soft Reset
        
    default:
        break;
    }
    
    return MPSC_ePowerState;
}

Global_PowerState_E MPSC_PowerState_Get(void)
{
    return MPSC_ePowerState;
}
