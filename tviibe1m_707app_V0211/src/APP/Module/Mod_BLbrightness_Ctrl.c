/******************************************************************************
;       Program  : Mod_BLBrightness_Ctrl.c
;       SWDD_ID  : SWDD_004
;       Project  : C120UAA01.0
;       Chip     : NXP S32K144
;       Date     : 2020 / 10 / 29
;       Author   : Derek
;       Describe : ���omponent靘��誘嚗�10ms�摰�ontrol backlight brightness嚗蒂隞卡imming�撘�脰�矽�PWM��
;                   1.	���omponent靘�璅�潘��璅�潸���潔�榆���128���矽���������潛撌桀�潮隞�128��
;                   2.	�璅�潸���潔�榆銝遛128嚗�誑�璅�潛隤踵������imming隤踵迤���潘�蝙�摰�����
;                   3.	�璅�潸���潔�榆銝遛128嚗��矽����澆摰1��
;                   4.	����1. ��閮�靘���10ms�嚗誑6ms�銵imming隤踹��
;                   5.	���omponent靘�璅�潮�摰�矽����誑���璅�潔�蝚�1.��2璇����矽�����
;
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
/* include global */
#include "gPinDef.h"
#include "Ford_Config.h"

/* include module */
#include "Mod_DetectI2C.h"
#include "Mod_BLBrightness_Ctrl.h"
#include "Mod_DataManagement.h"

/* Include App */
#include "TimerApp.h"
#include "PWMApp.h"
#include "I2CMasterApp.h"
/*------------------------ Declare External Var -----------------------------*/




static I2C_MESSAGES_T MBLBC_I2CMessage_t;
static PWMDimmingStep_T PWM_Adjuist_t = {0};
static uint16_t u16CurrentBLPWM_10bit = NUM_ZERO;
static uint8_t ucDimmingStepCnt = NUM_ZERO;
// static uint16_t PwmDimmingVal10bit_test=0U;
static uint16_t u16BLPWMVal_I2C_16bit = NUM_ZERO;
uint16_t u16PWMflage=0U;
/*---------------------------- Start Program ---------------------------------*/
/******************************************************************************
;       Function Name			:	MBLBC_PWM10bit_Merge
;       Function Description	:	Merge PWM[8:9]�WM[0:7]
;       Parameters				:	uint8_t ucBLPWMVal_8_9�int8_t ucBLPWMVal_0_7
;       Return Values			:	uint16_t u16BLPWMVal_10bit
;		Source ID				:	SWEDD_004_02
******************************************************************************/
uint16_t MBLBC_PWM10bit_Merge(uint8_t ucBLPWMVal_8_9,uint8_t ucBLPWMVal_0_7)
{
    uint16_t u16MergeResult = 0U;
    
    u16MergeResult |= ((uint16_t)(ucBLPWMVal_8_9) << PWMValMergeShift);
	u16MergeResult |= ucBLPWMVal_0_7;
    
    return u16MergeResult;
    
}

/******************************************************************************
;       Function Name			:	MBLBC_BLPWMDifference_Calculate
;       Function Description	:	Calculate difference of current PWM and target PWM,and set PWM Operator
;       Parameters				:	uint8_t ucBLPWMVal_8_9�int8_t ucBLPWMVal_0_7
;       Return Values			:	uint16_t u16DiffPwmValResult
;		Source ID				:	SWEDD_004_03
******************************************************************************/
uint16_t MBLBC_BLPWMDifference_Calculate(uint16_t u16CurrentBLPWM,uint16_t u16TargetBLPWM)
{
    uint16_t u16DiffPwmValResult = 0U;
    (void)u16DiffPwmValResult;
    
    if (u16TargetBLPWM > u16CurrentBLPWM)
    {
        /* code */
        u16DiffPwmValResult = u16TargetBLPWM - u16CurrentBLPWM;
        PWM_Adjuist_t.PWMOperator = eIncrease;
        u16PWMflage =0;
    }
    else if((u16TargetBLPWM < u16CurrentBLPWM))
    {
        /* code */
        u16DiffPwmValResult = u16CurrentBLPWM - u16TargetBLPWM;
        PWM_Adjuist_t.PWMOperator = eDecrease;
        u16PWMflage =1;
        
    }
    else
    {
        u16DiffPwmValResult = 0U;
    }
    
    
    return u16DiffPwmValResult;
    
}

/******************************************************************************
;	Function Name			:	bool TconApp_Backlight_Output(uint16_t u16DutyValue)
;	Function Description	:	Tcon Backlight Output
;	Parameters				:	[u16Value] - Backlight Brightness
;	Return Values			:	I2C : true or false
******************************************************************************/
uint8_t MBLC_TconApp_Backlight_Output(uint16_t u16DutyValue)
{
    uint8_t u8Idx = NUM_ZERO;
    uint8_t u8TconTxBuf[TCON_GLOBALDUTY_LENGTH + TCON_CHECKSUM_LENGTH];
    uint16_t u16CheckSum = NUM_ZERO;
    uint8_t u8Status = NUM_ZERO;
    
    u16CheckSum = TCON_GLOBAL_DUTY_ADDR_W;
    
    if(MMIM_LocalDim_Flag_Get() == NUM_ONE) 
    {
        /* local dimming is 14 bit */
        u8TconTxBuf[BLBC_DATABYTE_ONE] = (uint8_t)(u16DutyValue & 0xFCU); //0xF0 -> 0xFC
        u8TconTxBuf[BLBC_DATABYTE_TWO] = (uint8_t)((u16DutyValue>>8U) & 0xFFU);
        u8TconTxBuf[BLBC_DATABYTE_THR] = NUM_ZERO;
    }
    else 
    {
        /* global dimming is 12 bit */
        u8TconTxBuf[BLBC_DATABYTE_ONE] = (uint8_t)(u16DutyValue & 0xF0U);
        u8TconTxBuf[BLBC_DATABYTE_TWO] = (uint8_t)((u16DutyValue>>8U) & 0xFFU);
        u8TconTxBuf[BLBC_DATABYTE_THR] = NUM_ZERO;
    }
    
    
    //u8TconTxBuf[BLBC_DATABYTE_ONE] = (uint8_t)(u16DutyValue & 0xFCU); //0xF0 -> 0xFC
    //u8TconTxBuf[BLBC_DATABYTE_TWO] = (uint8_t)((u16DutyValue>>8U) & 0xFFU);
    //u8TconTxBuf[BLBC_DATABYTE_THR] = NUM_ZERO;
    //DEBUG_PF("write Tcon dimming value byte1=%x byte2=%x\r\n",u8TconTxBuf[BLBC_DATABYTE_ONE],u8TconTxBuf[BLBC_DATABYTE_TWO]);
    /* leo 2022_week6 modify , to get pwm value */    
#if 0
    uint8_t u8I2cBuffer[4] = {0};
#endif
    
    for(u8Idx = NUM_ZERO; u8Idx < BLBC_DATABYTE_THR; u8Idx++)
    {
        u16CheckSum += (uint16_t)u8TconTxBuf[u8Idx];
    }
    u8TconTxBuf[BLBC_DATABYTE_THR] = (uint8_t)(u16CheckSum & 0x00FFU);
    
    if (DRIVER_TRUE == I2CM_Write(TCON_ADDRESS_ID, TCON_GLOBAL_DUTY_ADDR_W, &u8TconTxBuf[BLBC_DATABYTE_ONE], sizeof(u8TconTxBuf)))
    {
        u8Status = true;
        
    }
    else
    {
        u8Status = false;
    }
    
    return u8Status;
}
/******************************************************************************
;       Function Name			:	MBLBC_BLPWM_Output
;       Function Description	:	Translate PWM resolution from 10bit to 16bit,and make PWM output to target value
;       Parameters				:	uint16_t u16TargetBLPWM_10bit
;       Return Values			:	uint16_t u16CurrentBLPWM_10bit
;		Source ID				:	SWEDD_004_04
******************************************************************************/
uint16_t MBLBC_BLPWM_Output(uint16_t u16TargetBLPWM_10bit)
{
    uint16_t u16BLPWMVal_16bit = NUM_ZERO;
    u16BLPWMVal_I2C_16bit = NUM_ZERO;
    
    
    u16BLPWMVal_16bit |= (u16TargetBLPWM_10bit << 5U); /*0~32768 for backlight duty 0~100%,(u16TargetBLPWM_10bit + 1) * 32  */
    if(u16BLPWMVal_16bit >= PERIOD_650HZ35)
    {
        u16BLPWMVal_16bit = PERIOD_650HZ35 + 1U;
    }
    
    if(u16TargetBLPWM_10bit >= MAX_10BIT_PWM_SET_VAlUE)
    {
#if BL_DUTY_70
	u16BLPWMVal_I2C_16bit = 42966;  /* for DV sample brightness out of spec, after PV sample need to remove 20220216 Joel*/
#else
        u16BLPWMVal_I2C_16bit = TCON_MAX_DUTY;
#endif
    }
    else
    {
#if BL_DUTY_70
	u16BLPWMVal_I2C_16bit |= ((u16TargetBLPWM_10bit) * 42); /* for DV sample brightness out of spec, after PV sample need to remove 20220216 Joel */
#else	
        u16BLPWMVal_I2C_16bit |= ((u16TargetBLPWM_10bit) << 6U);
#endif        
    }
    
    BL_Pwm_Duty_Output(BL_PWM_Channel,u16BLPWMVal_16bit);
    
    if(MMIM_SourceDriver_Ctrl(GETFLAG) == ENABLE)
    {
        
        (void)MBLC_TconApp_Backlight_Output(u16BLPWMVal_I2C_16bit);
    }
    
    u16CurrentBLPWM_10bit = u16TargetBLPWM_10bit;
    
    return u16CurrentBLPWM_10bit;
}


/******************************************************************************
;       Function Name			:	MBLBC_BalckLightDimming_Calculate
;       Function Description	:	Calculate how much PWM value every step will be changed
;       Parameters				:	uint16_t u16BLPWMDiffVal
;       Return Values			:	uint8_t PWM_Adjuist_t.DimmingCalculateFinish 
;		Source ID				:	SWEDD_004_05
******************************************************************************/
uint8_t MBLBC_BalckLightDimming_Calculate(uint16_t u16BLPWMDiffVal)
{
    if(u16BLPWMDiffVal >= 128U)
    {
        PWM_Adjuist_t.PWMpreStep = ((uint16_t)(u16BLPWMDiffVal+1U)*FIXEDPOINT_10) >> 7U ; /* FIXEDPOINT_10 1 -> 10 , /128 */
        PWM_Adjuist_t.ucDimmingStep = 128U;
        PWM_Adjuist_t.DimmingCalculateFinish = 1U;
    }
    else if (0U < u16BLPWMDiffVal && u16BLPWMDiffVal < 128U)
    {
        PWM_Adjuist_t.PWMpreStep = (uint16_t)FIXEDPOINT_10;
        PWM_Adjuist_t.ucDimmingStep = (uint8_t)u16BLPWMDiffVal;
        PWM_Adjuist_t.DimmingCalculateFinish = 1U;
    }
    else
    {
        PWM_Adjuist_t.DimmingCalculateFinish = 0U;
    }
    //DEBUG_PF("DIFF=%d, PWMSTEP=%d\r\n",u16BLPWMDiffVal, PWM_Adjuist_t.PWMpreStep);
    
    return PWM_Adjuist_t.DimmingCalculateFinish;
    
}

/******************************************************************************
;       Function Name			:	MBLBC_NonDimming_Set
;       Function Description	:	Set how much PWM every step will be adjusted at non dimming
;       Parameters				:	Noce
;       Return Values			:	uint8_t PWM_Adjuist_t.DimmingCalculateFinish 
;		Source ID				:	SWEDD_004_06
******************************************************************************/
uint8_t MBLBC_NonDimming_Set(void)
{  
    PWM_Adjuist_t.PWMpreStep = NUM_ZERO;
    PWM_Adjuist_t.ucDimmingStep = NUM_ZERO;
    PWM_Adjuist_t.DimmingCalculateFinish = 1;
    
    return PWM_Adjuist_t.DimmingCalculateFinish; 
}
/******************************************************************************
;       Function Name			:	MBLBC_NextStepBLPWM_Calculate
;       Function Description	:	Decide whitch Operator will be used,adjust 10bit resolution of PWM value 
;       Parameters				:	uint16_t u16CurrentPWMVal
;       Return Values			:	uint16_t u16NextStepBLPWMVal
;		Source ID				:	SWEDD_004_07
******************************************************************************/
uint16_t MBLBC_NextStepBLPWM_Calculate(uint16_t u16CurrentPWMVal)
{
    uint16_t u16NextStepBLPWMVal = 0U;
    
    if(PWM_Adjuist_t.PWMOperator == eIncrease)
    {
        u16NextStepBLPWMVal = u16CurrentPWMVal*FIXEDPOINT_10 + (ucDimmingStepCnt+1U)*(PWM_Adjuist_t.PWMpreStep);
        u16NextStepBLPWMVal = u16NextStepBLPWMVal/FIXEDPOINT_10;
    }
    else if (PWM_Adjuist_t.PWMOperator == eDecrease)
    {
        if((u16CurrentPWMVal*FIXEDPOINT_10) < (ucDimmingStepCnt+1U)*(PWM_Adjuist_t.PWMpreStep))
        {
            u16NextStepBLPWMVal = NUM_ZERO;
        }
        else
        {
            u16NextStepBLPWMVal = u16CurrentPWMVal*FIXEDPOINT_10 - (ucDimmingStepCnt+1U)*(PWM_Adjuist_t.PWMpreStep);
            u16NextStepBLPWMVal = u16NextStepBLPWMVal/FIXEDPOINT_10;         
        }
        
    }
    else
    {
        /* code */
    }
    
    return u16NextStepBLPWMVal;
}
/******************************************************************************
;       Function Name			:	MBLBC_BLPWMTargetVal_Set
;       Function Description	:	Set PWM target value
;       Parameters				:	uint16_t u16BLPWMtargetVal
;       Return Values			:	uint8_t ucProcessStatus
;		Source ID				:	SWEDD_004_08
******************************************************************************/
uint8_t MBLBC_BLPWMTargetVal_Set(uint16_t u16BLPWMtargetVal)
{
	uint8_t ucProcessStatus = NUM_ZERO;
    
	PWMDimmingStep_T Dimming = PWM_Adjuist_t;
    
    if(u16BLPWMtargetVal >= 0x3ffU)
    {
        
        Dimming.u16TargetBLPWM_10bit = 0x400U;
        
    }
    else
    {
        Dimming.u16TargetBLPWM_10bit = u16BLPWMtargetVal;
    }
    
    PWM_Adjuist_t = Dimming;
    
    ucProcessStatus = NUM_ONE; 
	
    return ucProcessStatus;
    
}       	

/******************************************************************************
;       Function Name			:	MBLBC_PWMDimming_6ms
;       Function Description	:	Excute process at timer 2ms ISR
;       Parameters				:	uint16_t u16BLPWMDiffVal
;       Return Values			:	None
;		Source ID				:	SWEDD_004_08
******************************************************************************/
void MBLBC_PWMDimming_6ms(void* context)
{
    UNUSED (context);
    
    uint16_t PwmDimmingVal10bit=0U;
    static uint16_t u16NowPWMVal = 0U;
    static uint8_t MBLBC_Count_6ms = NUM_ZERO;

    if(MBLBC_Count_6ms == 2U)
    {
      
        PWMDimmingStep_T Dimming = PWM_Adjuist_t;
        
        if(Dimming.DimmingCalculateFinish == NUM_ONE)
        {
            ucDimmingStepCnt = 0;
            u16NowPWMVal = u16CurrentBLPWM_10bit;
        }
        if(ucDimmingStepCnt < Dimming.ucDimmingStep)
        {
            Dimming.DimmingActionFlag = true;
            PwmDimmingVal10bit = MBLBC_NextStepBLPWM_Calculate(u16NowPWMVal);
            
            MBLBC_BLPWM_Output(PwmDimmingVal10bit);
            //DEBUG_PF("COUNT=%d ,BLPWM=%d\r\n",ucDimmingStepCnt,PwmDimmingVal10bit);
            ucDimmingStepCnt++;
            
        }
        else if(ucDimmingStepCnt == Dimming.ucDimmingStep)
        {
            // PwmDimmingVal10bit_test = Dimming.u16TargetBLPWM_10bit;
            MBLBC_BLPWM_Output(Dimming.u16TargetBLPWM_10bit);
            //DEBUG_PF("COUNT=%d ,BLPWM=%d\r\n",ucDimmingStepCnt,Dimming.u16TargetBLPWM_10bit);
            ucDimmingStepCnt++;
            
        }
        else
        {
            Dimming.DimmingActionFlag = false;
        }
        
        Dimming.DimmingCalculateFinish = NUM_ZERO;
        
        PWM_Adjuist_t = Dimming;
        
        MBLBC_Count_6ms =NUM_ZERO;
    }
    else
    {
        MBLBC_Count_6ms++;
    }
    
    
}

/******************************************************************************
;       Function Name			:	MBLBC_BLBrightness_Ctrl
;       Function Description	:	Control BL brightness with dimmong or nondimming 
;       Parameters				:	uint8_t ucBLPWMVal_8_9,uint8_t ucBLPWMVal_0_7,uint8_t ucDimmingEnable
;       Return Values			:	uint8_t ucBLBC_Process_Status
;		Source ID				:	SWEDD_004_01
******************************************************************************/
MBLBC_BLBrightness_status_E MBLBC_BLBrightness_Ctrl(uint8_t ucBLPWMVal_8_9,uint8_t ucBLPWMVal_0_7,uint8_t ucDimmingEnable)
{
    MBLBC_BLBrightness_status_E ucBLBC_Process_Status = eProcess_Ready;
    static uint16_t u16PWMValMerged_10bit = NUM_ZERO;
    uint16_t u16PWMValDifference_10bit = NUM_ZERO;
    
    /* Mearge TargetVal */
    u16PWMValMerged_10bit = MBLBC_PWM10bit_Merge(ucBLPWMVal_8_9,ucBLPWMVal_0_7);
    
    /* leo 20211126 */
    if(MMIM_VoltageDRT_Flag_Get() == true)
    {
        if(u16PWMValMerged_10bit > 532)    /* 532 = 52 PA */
        {
            u16PWMValMerged_10bit = 532;
        }
    }
    
    /* Set target value */
    MBLBC_BLPWMTargetVal_Set(u16PWMValMerged_10bit);
    (void)MMIM_PWMTargetValue_Ctrl(u16PWMValMerged_10bit); 
    
    /* Calculate difference of current PWM and target PWM */
    u16PWMValDifference_10bit = MBLBC_BLPWMDifference_Calculate(u16CurrentBLPWM_10bit,u16PWMValMerged_10bit);
   
    
    /* Calculate how much PWM value every step will be changed */
    if(u16PWMValDifference_10bit != NUM_ZERO)
    {
        if(ucDimmingEnable == ENABLE)
        {
            MBLBC_I2CMessage_t = *MMIM_pI2CMessage_Get();
            /*set BL status is high*/
            if( DISABLE == MBLBC_I2CMessage_t.display_status.BL_ST)
            {
                MMIM_DisplayStatusReg_Ctrl(SOUREFROMI2C,eBacklightStatus,true);
                //DEBUG_PF("BL value=true , BL ST=High\r\n"); 
            }
            MBLBC_BalckLightDimming_Calculate(u16PWMValDifference_10bit);
            //MBLBC_BLPWM_Output(u16PWMValMerged_10bit);
            //DEBUG_PF("u16PWMValMerged_10bit=%x\r\n",u16PWMValMerged_10bit);
            
            /*  Set Process_Status*/
            ucBLBC_Process_Status = eDimmingCal_finished;
        }
        else
        {
            /* code */
            MBLBC_NonDimming_Set();
            
            ucBLBC_Process_Status = eNonDimmingCtrl_finished;
        }   
    }
    else
    {
        /* code */
    }
    
    if( NUM_ZERO == ucBLPWMVal_8_9 &&  NUM_ZERO == ucBLPWMVal_0_7)
    {
        MMIM_DisplayStatusReg_Ctrl(SOUREFROMI2C,eBacklightStatus,false);
        //DEBUG_PF("BL value=0 , BL ST=low\r\n");    
    }


    (void)MMIM_BLPWMFlag_Ctrl(DISABLE);
    
    return ucBLBC_Process_Status;
}

uint8_t Get_Dimming_Action_flag(void)
{
  return PWM_Adjuist_t.DimmingActionFlag;
}

/* 2ms timer ISR */
void MBLC_IRQCallBack_Install(void)
{
    /* Create PWM Timer for 1ms */
    HAL_Timer_Init(MBLBC_PWMDimming_TIMER_2MINSEC,&gts_2mSec_Timer_Continuous_Config);  //2ms
    
    /*Timer register callback*/
    HAL_Timer_Callback_Register (MBLBC_PWMDimming_TIMER_2MINSEC,MBLBC_PWMDimming_6ms,NULL);
    
    /*Timer Enable*/
    HAL_Timer_Active(MBLBC_PWMDimming_TIMER_2MINSEC  ,FUNC_ENABLE);
    
    /*PWM_TIMER & COUNTER_FLAG_TIMER timer start*/ 
    HAL_Timer_Start(MBLBC_PWMDimming_TIMER_2MINSEC   ,FUNC_ENABLE);
    
    /*Enable PWM_TIMER interrupt & recounting */
    HAL_Timer_Interrupt_Set (MBLBC_PWMDimming_TIMER_2MINSEC, FUNC_ENABLE, CYHAL_TCPWM_IRQ_PRIORITY);   
}

