/**
* @file Com_DeviceControl.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-18
* 
* @copyright Copyright (c) 2020
* 
*/

/* include public information */
#include "gPinDef.h"
#include "UartApp.h"

/* include conponent */
#include "Com_PowerManagement.h"
#include "Com_Communication.h"
#include "Com_DeviceControl.h"

/* include module */
#include "Mod_BLEN_Ctrl.h"
#include "Mod_BLBrightness_Ctrl.h"
#include "Mod_DataManagement.h"
#include "Mod_ScanDirection_Ctrl.h"
#include "Mod_DisplayEN_Ctrl.h"
#include "Mod_FPNCtrl.h"

/* Include App */
#include "WDTApp.h"

static CDC_EVENT_T CDC_Event_t={eDisplayEN,{NUM_ZERO}};

/* declare global value*/
static Global_PowerState_E eCDC_PowerState = ePowerState_Stop;
static Global_LockState_E eCDC_LockState = eLocked;
static uint8_t CDC_DisplayStatus = DISABLE;

static I2C_MESSAGES_T CDC_I2CMessage_t;
/*state machine */

CDC_StateMachine_E CDC_StateMachine_e = CDC_STATE_INIT;

/* Start Program */

/**
* @brief 
* 
*/
uint8_t  CDC_Event_Get(CDC_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {
        memcpy(pEvent_t,&CDC_Event_t,sizeof(CDC_EVENT_T));
        ProcessStatus = true;
    }
    
    return ProcessStatus;
}


/**
* @brief 
* 
*/
uint8_t  CDC_Event_Set(CDC_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {
        memcpy(&CDC_Event_t,pEvent_t,sizeof(CDC_EVENT_T));
        ProcessStatus = true;
    }
    
    return ProcessStatus;
}



Global_PowerState_E CDC_DisplaySequence_Ctrl(CDC_DispSeq_E DisplayEN)
{
    
    eCDC_PowerState = MMIM_PowerState_Ctrl(ePowerstate_Get);
    eCDC_LockState = (Global_LockState_E)MMIM_LockPinStatus_Ctrl(GETFLAG);   
    
    switch(DisplayEN)
    {
        case eDispOffSeq:
            
            (void)MBLBC_BLBrightness_Ctrl(NUM_ZERO,NUM_ZERO,DISABLE);
             //DEBUG_PF("======== DispOffq BL PWM=0 =========\r\n");
            CDC_DisplayStatus = MBLC_Display_Ctrl(DISABLE,eCDC_PowerState,eCDC_LockState);
            
            (void)MMIM_SourceDriver_Ctrl(DISABLE);
            CDC_DisplayStatus |= MDEN_DispENProcess_Ctrl(DISABLE,eCDC_PowerState,eCDC_LockState) << SHIFT_BIT_1;
            
            if((CDC_DisplayStatus == Display_Disable) && (eCDC_PowerState != ePowerState_HostShutdown))
            {
                MMIM_DisplayStatusReg_Ctrl(SOUREFROMI2C,eBacklightStatus,LEVEL_LOW);
                eCDC_PowerState = ePowerState_Standby;
                // (void)MMIM_PowerState_Ctrl(eCDC_PowerState);
            }
            
            break;
            
        case eDispOnSeq :
            
            //HAL_GPIO_PinWrite(PIN_MCU_PIN64_P17_0,PIN_LOW);     // PIN_MCU_PIN64_P17_0 =0
            CDC_DisplayStatus = MDEN_DispENProcess_Ctrl(ENABLE,eCDC_PowerState,eCDC_LockState);				
            CDC_DisplayStatus |= (MBLC_Display_Ctrl(ENABLE,eCDC_PowerState,eCDC_LockState) << SHIFT_BIT_1) ;
            
            if((CDC_DisplayStatus ==  DISPNORMALDATA) && (eCDC_PowerState == ePowerState_Standby))
            {
                (void)MBLBC_BLBrightness_Ctrl(NUM_ZERO,NUM_ZERO,DISABLE);
                /*michael*/
                //MMIM_DisplayStatusReg_Ctrl(SOURENOTFROMI2C,eBacklightStatus,LEVEL_HIGH);
                
                MMIM_DisplayStatusReg_Ctrl(SOURENOTFROMI2C,eDisplayInitialized,LEVEL_HIGH);
                
                // PIN_MCU_PIN64_P17_0 =1(INT_ERR=1)
                //MDEN_SourceDriver_DispOn_CheckReset();
                
                /* Clears ("feeds") the watchdog, to prevent a XRES device reset. */ 
                WDTApp_ClearWDT (); 
                //Cy_SysLib_Delay(500);  // Delay 500 msec,Normal Run mode  
                //MDEN_SourceDriver_DispOn_CheckReset();
                
                //Cy_SysLib_Delay(200);  // Delay 500 msec,Normal Run mode  
                //WDTApp_ClearWDT (); 
                
                //MDEN_SourceDriver_DispOn_CheckReset();
                (void)MMIM_SourceDriver_Ctrl(ENABLE);  
                
                eCDC_PowerState = ePowerState_Normal;
                // (void)MMIM_PowerState_Ctrl(eCDC_PowerState);    
                
                if(eCDC_PowerState == ePowerState_Normal)
                {	
                    CDC_I2CMessage_t = *MMIM_pI2CMessage_Get();
                    /* Set buffer to adjust backlight duty*/
                    CDC_Event_t.Event_e |= eBLBrightnessCtrl;
                    CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_ONE] = CDC_I2CMessage_t.lcd_backlight_pwm_value.BL_PWM_9_8;
                    CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_TWO] = CDC_I2CMessage_t.lcd_backlight_pwm_value.BL_PWM_7_0;
                    CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_THR] = ENABLE;
                }
                
            }  
            
            break;
            
        default:
            break;
    }
    
    return eCDC_PowerState;
}



/**
* @brief 
* 
*/
static void CDC_Event_Process(void)
{ /* Init State, init somett_Process(void)*/
    CC_EVENT_T CDC_CC_EVENT_t={eDisplayEN,{NUM_ZERO}};
    
    uint8_t QueueDataBuff[CDC_EVENT_BUFF_SIZE];
    
    eCDC_PowerState = MMIM_PowerState_Ctrl(ePowerstate_Get);
    eCDC_LockState = (Global_LockState_E)MMIM_LockPinStatus_Ctrl(GETFLAG);
    
    
    /* Get communication Event to enable display */
    if(CC_Event_Get(&CDC_CC_EVENT_t) == true)
    {
        /* Get display enable flag */
        if((CDC_CC_EVENT_t.Event_e & (uint8_t)CDC_DISPEN_EVENT_MSK) == (uint8_t)CDC_DISPEN_EVENT_MSK)
        {
            memcpy(QueueDataBuff,CDC_CC_EVENT_t.EventBuff, CC_EVENT_BUFF_SIZE_4);
            
            if((QueueDataBuff[CDC_BUFFBYTE_ONE] & MASK_1_BIT) == MASK_1_BIT)
            {     
                //HAL_GPIO_PinWrite(PIN_MCU_PIN64_P17_0,PIN_LOW);     // PIN_MCU_PIN64_P17_0 =0           
                
                eCDC_PowerState = CDC_DisplaySequence_Ctrl(eDispOnSeq);
                
                //HAL_GPIO_PinWrite(PIN_MCU_PIN64_P17_0,PIN_HIGH);                
            }
            else
            {   
                eCDC_PowerState = CDC_DisplaySequence_Ctrl(eDispOffSeq); /* 20220311 Joel test close display off */
            }         
			(void)MMIM_PowerState_Ctrl(eCDC_PowerState);
            
            CDC_CC_EVENT_t.Event_e &= (~ CDC_DISPEN_EVENT_MSK);
            CDC_CC_EVENT_t.EventBuff[CDC_BUFFBYTE_ONE] = NUM_ZERO;
            (void)CC_Event_Set(&CDC_CC_EVENT_t );
        }
        
        /* Get BIST Mode enable flag */
        if((CDC_CC_EVENT_t.Event_e & (uint8_t)CDC_BISTMOSE_EVENT_MSK) == (uint8_t)CDC_BISTMOSE_EVENT_MSK)
        {
            memcpy(QueueDataBuff,CDC_CC_EVENT_t.EventBuff + CC_MEM_SHIFT16, CC_EVENT_BUFF_SIZE_4);
            
            MDEN_TConBistMode_En(QueueDataBuff[CDC_BUFFBYTE_ONE]);	        
            
            CDC_CC_EVENT_t.Event_e &= (~CDC_BISTMOSE_EVENT_MSK); 
            (void)CC_Event_Set(&CDC_CC_EVENT_t );
        }
        
        /* Get Backlight PWM  enable flag */
        if((CDC_CC_EVENT_t.Event_e & (uint8_t)CDC_BLEN_EVENT_MSK) == (uint8_t)CDC_BLEN_EVENT_MSK)
        {
            
            memcpy(QueueDataBuff,CDC_CC_EVENT_t.EventBuff + CC_MEM_SHIFT4, CC_EVENT_BUFF_SIZE_4);
            
            if(eCDC_LockState == eLocked && eCDC_PowerState == ePowerState_Normal)
            {             
                (void)MBLBC_BLBrightness_Ctrl(QueueDataBuff[BLBC_DATABYTE_ONE],QueueDataBuff[BLBC_DATABYTE_TWO],QueueDataBuff[BLBC_DATABYTE_THR]);
            }
            CDC_CC_EVENT_t.Event_e &= (~CDC_BLEN_EVENT_MSK); 
            (void)CC_Event_Set(&CDC_CC_EVENT_t );
        }  
        
        /* Get SCANDIRECT  enable flag */
        if((CDC_CC_EVENT_t.Event_e & (uint8_t)CDC_SCANDIRECT_EVENT_MSK) == (uint8_t)CDC_SCANDIRECT_EVENT_MSK)
        {
            memcpy(QueueDataBuff,CDC_CC_EVENT_t.EventBuff + CC_MEM_SHIFT8, CC_EVENT_BUFF_SIZE_4);
            
            (void)MSDC_ScanDirection_Ctrl((DisplayScanStatus_E )QueueDataBuff[MSDC_DATABYTE_ONE]);
            CDC_CC_EVENT_t.Event_e &= (~CDC_SCANDIRECT_EVENT_MSK); 
            (void)CC_Event_Set(&CDC_CC_EVENT_t );
        }  
        
        /* Get Diagnostic Message flag  */
        if((CDC_CC_EVENT_t.Event_e & (uint8_t)CDC_DIAGNOSTICMESSAGE_EVENT_MSK) == (uint8_t)CDC_DIAGNOSTICMESSAGE_EVENT_MSK)
        {
            memcpy(QueueDataBuff,CDC_CC_EVENT_t.EventBuff + CC_MEM_SHIFT20, CC_EVENT_BUFF_SIZE_25);
            
            //(void)MSDC_ScanDirection_Ctrl((DisplayScanStatus_E )QueueDataBuff[MSDC_DATABYTE_ONE]);
            CDC_CC_EVENT_t.Event_e &= (~CDC_DIAGNOSTICMESSAGE_EVENT_MSK); 
            (void)CC_Event_Set(&CDC_CC_EVENT_t );
        } 
        
        
        
    }
    
    /* leo 20211125 */
    /*
    if voltage flag is set, check current target is lower than 52% duty
    if yes, do nothing
    if no , set brightness to 52%
    */
#if 1
    if(MMIM_VoltageDRT_Flag_Get() == true)
    {
        if(MMIM_PWMTargetValue_Ctrl(GETPWMFLAG) > 532)  /* 532/1024 = 52% duty pwm */
        {
            /*
            0000 0010           0001 0100
            532 = 02                  14 
            */
            (void)MBLBC_BLBrightness_Ctrl(2, 14, ENABLE);
        }
    }   
    else
    {
        ;
    }
#endif    
    
    /* Get Device control Event */
    if(CDC_Event_t.Event_e > NUM_ZERO)
    {
        /* Get display enable flag */
        if((CDC_Event_t.Event_e & eDisplayEN) == eDisplayEN)
        {
            CDC_DisplayStatus = MDEN_DispENProcess_Ctrl(CDC_Event_t.EventBuff[CDC_BUFFBYTE_ONE],eCDC_PowerState,eCDC_LockState);
            CDC_Event_t.Event_e &= (~ eDisplayEN);
            CDC_Event_t.EventBuff[CDC_BUFFBYTE_ONE] = NUM_ZERO;
        }
        
        /* Get eDisplayEN_Sequence enable flag */
        if((CDC_Event_t.Event_e & eDisplayEN_Sequence) == eDisplayEN_Sequence)
        {       
            if(CDC_Event_t.EventBuff[CDC_MEM_SHIFT15 + CDC_BUFFBYTE_ONE] == ENABLE)
            {     
               	eCDC_PowerState = CDC_DisplaySequence_Ctrl(eDispOnSeq);        
            }
            else
            {   
                eCDC_PowerState = CDC_DisplaySequence_Ctrl(eDispOffSeq);  /* 20220311 Joel test close display off */
                //DEBUG_PF("eDisplayEN_Sequence enable flag = low\r\n");
            } 
            (void)MMIM_PowerState_Ctrl(eCDC_PowerState); 
            
            CDC_Event_t.Event_e &= (~ eDisplayEN_Sequence);
            CDC_Event_t.EventBuff[CDC_MEM_SHIFT15 + CDC_BUFFBYTE_ONE] = NUM_ZERO;
        }
        
        /* Get Backlight enable flag */
        if((CDC_Event_t.Event_e & eBacklightEN) == eBacklightEN)
        {     
            (void)MBLC_Display_Ctrl(CDC_Event_t.EventBuff[CDC_MEM_SHIFT3 + CDC_BUFFBYTE_ONE] ,eCDC_PowerState,eCDC_LockState);                
            
            CDC_Event_t.Event_e &= (~ eBacklightEN);
            CDC_Event_t.EventBuff[CDC_MEM_SHIFT3 + CDC_BUFFBYTE_ONE] = NUM_ZERO;
        }
        
        /* Get eBLBrightnessCtrl enable flag */
        if((CDC_Event_t.Event_e & eBLBrightnessCtrl) == eBLBrightnessCtrl)
        { 
            if(eCDC_LockState == eLocked)
            {
                (void)MBLBC_BLBrightness_Ctrl(CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + BLBC_DATABYTE_ONE],
                                              CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + BLBC_DATABYTE_TWO],CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + BLBC_DATABYTE_THR]);
            }  
            
            CDC_Event_t.Event_e &= (~ eBLBrightnessCtrl);
            CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + BLBC_DATABYTE_ONE] = NUM_ZERO;
            CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + BLBC_DATABYTE_TWO] = NUM_ZERO;
            CDC_Event_t.EventBuff[CDC_MEM_SHIFT6 + BLBC_DATABYTE_THR] = NUM_ZERO;
        }
        
        /* Get eFPN_Ctrl enable flag */
        if((CDC_Event_t.Event_e & eFPN_Ctrl) == eFPN_Ctrl)
        {
            if((CDC_Event_t.EventBuff[CDC_MEM_SHIFT21 + CDC_BUFFBYTE_ONE] & MASK_1_BIT) == MASK_1_BIT)
            {     
                ;                
            }
            else
            {   
                ;
            }         
            
            CDC_Event_t.Event_e &= (~ eFPN_Ctrl);
            CDC_Event_t.EventBuff[CDC_MEM_SHIFT21 + CDC_BUFFBYTE_ONE] = NUM_ZERO;
        }        
    }
    
    
    
}

/*
* @brief 
* 
*/
static void CDC_Service_Init(void)
{
    /* Module initial */
 	eCDC_PowerState = MMIM_PowerState_Ctrl(ePowerstate_Get);
	CDC_StateMachine_e = CDC_STATE_PROCESS;
   	MBLC_IRQCallBack_Install(); 
}


/**
* @brief 
* 
*/
static void CDC_Service_Data_Process(void)
{   
	static uint8_t TimeOutCnt = (uint8_t)CDC_COUNTER_500; /*TimeOut for 500ms*/
    
	FPN_T CDC_FPN_t_DelAssm = {0};  // Delivery_assembly
        FPN_T CDC_FPN_t_CoreAssm = {0}; // Core_assembly
        FPN_T CDC_FPN_t_MainCalib = {0};  // Main_calibration
        FPN_DISPLAY_ID_T CDC_FPN_t_DispID = {0};  // Display_ID
        FPN_T CDC_FPN_t_SoftWareFPN = {0};  // Software Ford Part Number 
        FPN_T CDC_FPN_t_SerailFPN = {0}; /* leo 20220621 */
        
        uint8_t FPN_DelAssm_Control_Status = NUM_ZERO;
        uint8_t FPN_CoreAssm_Control_Status = NUM_ZERO;
        uint8_t FPN_MainCalib_Control_Status = NUM_ZERO;
        uint8_t FPN_DispID_Control_Status = NUM_ZERO;
        uint8_t FPN_Software_Control_Status = NUM_ZERO;
        uint8_t FPN_Serial_Control_Status = NUM_ZERO;  /* leo 20220621 */
    
	(void)TimeOutCnt;
    
    
	TimeOutCnt--;
	/* Star Process */
   	eCDC_PowerState = MMIM_PowerState_Ctrl(ePowerstate_Get);
	eCDC_LockState = (Global_LockState_E)MMIM_LockPinStatus_Ctrl(GETFLAG);
	CDC_I2CMessage_t = *MMIM_pI2CMessage_Get();
    
        CDC_Event_Process();
#if 1   
	/* Set object */
	CDC_FPN_t_DelAssm.FPN_Flag_t.FPNCtrlFlag = MMIM_DelAssmPNStatusRegFlag_Ctrl(GETFLAG);
	CDC_FPN_t_DelAssm.FPN_Flag_t.FPNDataFlag = MMIM_WriteDelAssmFPNFlag_Ctrl(GETFLAG);
	CDC_FPN_t_DelAssm.FPN_Status_t.FPN_WRT_ST = CDC_I2CMessage_t.pn_delivery_assembly_status_ctrl.WRT_ST;
	CDC_FPN_t_DelAssm.FPNDataLength = FPN_SIZE;
	CDC_FPN_t_DelAssm.pFPNDataBuff = MMIM_pdelivery_assembly_Get();
	
	/* excute FPN process */
	FPN_DelAssm_Control_Status = MFPNC_FPNDelAssmControl_Process(&CDC_FPN_t_DelAssm);
    
	switch (FPN_DelAssm_Control_Status)
	{
    case FPN_WRITE_SUCCESS:
        /* code */
        MMIM_DelAssmPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteDelAssmFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_delivery_assembly_status_ctrl.INT_WRT = ENABLE;
        MMIM_pI2CMessage_Get()->pn_delivery_assembly_status_ctrl.INT_WRT = CDC_I2CMessage_t.pn_delivery_assembly_status_ctrl.INT_WRT;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_CHECKSUM_ERR:
        /* code */
        MMIM_DelAssmPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteDelAssmFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_delivery_assembly_status_ctrl.CKSUM_ERR = ENABLE;
        MMIM_pI2CMessage_Get()->pn_delivery_assembly_status_ctrl.CKSUM_ERR = CDC_I2CMessage_t.pn_delivery_assembly_status_ctrl.CKSUM_ERR;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_WRITE_FAIL:
        /* code */
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        //MMIM_WriteDelAssmFPNFlag_Ctrl(DISABLE);
        
        break;
    case FPN_READ_SUCCESS:
        /* code */
        memcpy(MMIM_pI2CMessage_Get()->delivery_assembly,CDC_FPN_t_DelAssm.ReadFPNBuff,FPN_SIZE);
        break;
    default:
        break;
	}
    
	/* Set object */
	CDC_FPN_t_CoreAssm.FPN_Flag_t.FPNCtrlFlag = MMIM_CoreAssmPNStatusRegFlag_Ctrl(GETFLAG);
	CDC_FPN_t_CoreAssm.FPN_Flag_t.FPNDataFlag = MMIM_WriteCoreAssmFPNFlag_Ctrl(GETFLAG);
	CDC_FPN_t_CoreAssm.FPN_Status_t.FPN_WRT_ST = CDC_I2CMessage_t.pn_core_assembly_status_ctrl.WRT_ST;
	CDC_FPN_t_CoreAssm.FPNDataLength = FPN_SIZE;
	CDC_FPN_t_CoreAssm.pFPNDataBuff = MMIM_pcore_assembly_Get();
	
	/* excute FPN process */
	FPN_CoreAssm_Control_Status = MFPNC_FPNCoreAssmControl_Process(&CDC_FPN_t_CoreAssm);
    
	switch (FPN_CoreAssm_Control_Status)
	{
    case FPN_WRITE_SUCCESS:
        /* code */
        MMIM_CoreAssmPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteCoreAssmFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_core_assembly_status_ctrl.INT_WRT = ENABLE;
        MMIM_pI2CMessage_Get()->pn_core_assembly_status_ctrl.INT_WRT = CDC_I2CMessage_t.pn_core_assembly_status_ctrl.INT_WRT;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_CHECKSUM_ERR:
        /* code */
        MMIM_CoreAssmPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteCoreAssmFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_core_assembly_status_ctrl.CKSUM_ERR = ENABLE;
        MMIM_pI2CMessage_Get()->pn_core_assembly_status_ctrl.CKSUM_ERR = CDC_I2CMessage_t.pn_core_assembly_status_ctrl.CKSUM_ERR;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_WRITE_FAIL:
        /* code */
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        // MMIM_WriteCoreAssmFPNFlag_Ctrl(DISABLE);
        
        break;
    case FPN_READ_SUCCESS:
        /* code */
        memcpy(MMIM_pI2CMessage_Get()->core_assembly,CDC_FPN_t_CoreAssm.ReadFPNBuff,FPN_SIZE);
        break;
    default:
        break;
	}
    
	/* Set object */
	CDC_FPN_t_MainCalib.FPN_Flag_t.FPNCtrlFlag = MMIM_MainCalibPNStatusRegFlag_Ctrl(GETFLAG);
	CDC_FPN_t_MainCalib.FPN_Flag_t.FPNDataFlag = MMIM_WriteMainCalibFPNFlag_Ctrl(GETFLAG);
	CDC_FPN_t_MainCalib.FPN_Status_t.FPN_WRT_ST = CDC_I2CMessage_t.pn_main_calibration_status_ctrl.WRT_ST;
	CDC_FPN_t_MainCalib.FPNDataLength = FPN_SIZE;
	CDC_FPN_t_MainCalib.pFPNDataBuff = MMIM_pmain_calibration_Get();
	
	/* excute FPN process */
	FPN_MainCalib_Control_Status = MFPNC_FPNMainCalibControl_Process(&CDC_FPN_t_MainCalib);
    
	switch (FPN_MainCalib_Control_Status)
	{
    case FPN_WRITE_SUCCESS:
        /* code */
        MMIM_MainCalibPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteMainCalibFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_main_calibration_status_ctrl.INT_WRT = ENABLE;		
        MMIM_pI2CMessage_Get()->pn_main_calibration_status_ctrl.INT_WRT = CDC_I2CMessage_t.pn_main_calibration_status_ctrl.INT_WRT;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_CHECKSUM_ERR:
        /* code */
        MMIM_MainCalibPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteMainCalibFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_main_calibration_status_ctrl.CKSUM_ERR = ENABLE;	
        MMIM_pI2CMessage_Get()->pn_main_calibration_status_ctrl.CKSUM_ERR = CDC_I2CMessage_t.pn_main_calibration_status_ctrl.CKSUM_ERR;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_WRITE_FAIL:
        /* code */
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        //MMIM_WriteMainCalibFPNFlag_Ctrl(DISABLE);
        
        break;
    case FPN_READ_SUCCESS:
        /* code */
        memcpy(MMIM_pI2CMessage_Get()->main_calibration_ford_part_number,CDC_FPN_t_MainCalib.ReadFPNBuff,FPN_SIZE);
        break;
    default:
        break;
	}
    
	/* Set object */
	CDC_FPN_t_DispID.FPN_Flag_t.FPNCtrlFlag = MMIM_DispIDPNStatusRegFlag_Ctrl(GETFLAG);
	CDC_FPN_t_DispID.FPN_Flag_t.FPNDataFlag = MMIM_WriteDispIDFPNFlag_Ctrl(GETFLAG);
	CDC_FPN_t_DispID.FPN_Status_t.FPN_WRT_ST = CDC_I2CMessage_t.pn_display_identification_status_ctrl.WRT_ST;
	CDC_FPN_t_DispID.FPNDataLength = FPN_DISP_ID_SIZE;
	CDC_FPN_t_DispID.pFPNDataBuff = MMIM_pdisplay_id_Get();
	
	/* excute FPN process */
	FPN_DispID_Control_Status = MFPNC_FPNDispIDControl_Process(&CDC_FPN_t_DispID);
    
	switch (FPN_DispID_Control_Status)
	{
    case FPN_WRITE_SUCCESS:
        /* code */
        MMIM_DispIDPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteDispIDFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_display_identification_status_ctrl.INT_WRT = ENABLE;			
        MMIM_pI2CMessage_Get()->pn_display_identification_status_ctrl.INT_WRT = CDC_I2CMessage_t.pn_display_identification_status_ctrl.INT_WRT;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_CHECKSUM_ERR:
        /* code */
        MMIM_DispIDPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteDispIDFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_display_identification_status_ctrl.CKSUM_ERR = ENABLE;
        MMIM_pI2CMessage_Get()->pn_display_identification_status_ctrl.CKSUM_ERR = CDC_I2CMessage_t.pn_display_identification_status_ctrl.CKSUM_ERR;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_WRITE_FAIL:
        /* code */
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        //MMIM_WriteDispIDFPNFlag_Ctrl(DISABLE);
        
        break;
    case FPN_READ_SUCCESS:
        /* code */
        memcpy((uint8_t *)(&MMIM_pI2CMessage_Get()->display_identification),CDC_FPN_t_DispID.ReadFPNBuff,FPN_DISP_ID_SIZE);
        break;
    default:
        break;
	}
    
	/* Set object */
	CDC_FPN_t_SoftWareFPN.FPN_Flag_t.FPNCtrlFlag = MMIM_SoftwarePNStatusRegFlag_Ctrl(GETFLAG);
	CDC_FPN_t_SoftWareFPN.FPN_Flag_t.FPNDataFlag = MMIM_WriteSoftwareFPNFlag_Ctrl(GETFLAG);
	CDC_FPN_t_SoftWareFPN.FPN_Status_t.FPN_WRT_ST = CDC_I2CMessage_t.pn_software_ford_part_number_status_ctrl.WRT_ST;
	CDC_FPN_t_SoftWareFPN.FPNDataLength = FPN_SIZE;
	CDC_FPN_t_SoftWareFPN.pFPNDataBuff = MMIM_psoftware_fpn_Get();
	
	/* excute FPN process */
    FPN_Software_Control_Status = MFPNC_FPNSoftwareControl_Process(&CDC_FPN_t_SoftWareFPN);
    
	switch (FPN_Software_Control_Status)
	{
    case FPN_WRITE_SUCCESS:
        /* code */
        MMIM_SoftwarePNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteSoftwareFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_software_ford_part_number_status_ctrl.INT_WRT = ENABLE;
        MMIM_pI2CMessage_Get()->pn_software_ford_part_number_status_ctrl.INT_WRT = CDC_I2CMessage_t.pn_software_ford_part_number_status_ctrl.INT_WRT;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_CHECKSUM_ERR:
        /* code */
        MMIM_SoftwarePNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteSoftwareFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_software_ford_part_number_status_ctrl.CKSUM_ERR = ENABLE;			
        MMIM_pI2CMessage_Get()->pn_software_ford_part_number_status_ctrl.CKSUM_ERR = CDC_I2CMessage_t.pn_software_ford_part_number_status_ctrl.CKSUM_ERR ;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_WRITE_FAIL:
        /* code */
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        //MMIM_WriteSoftwareFPNFlag_Ctrl(DISABLE);
        
        break;
    case FPN_READ_SUCCESS:
        /* code */
        memcpy(MMIM_pI2CMessage_Get()->software_ford_part_number,CDC_FPN_t_SoftWareFPN.ReadFPNBuff,FPN_SIZE);
        break;
    default:
        break;
	}

        /* leo 20220621 */
        /* Set object */
	CDC_FPN_t_SerailFPN.FPN_Flag_t.FPNCtrlFlag = MMIM_SerialPNStatusRegFlag_Ctrl(GETFLAG);
	CDC_FPN_t_SerailFPN.FPN_Flag_t.FPNDataFlag = MMIM_WriteSerialFPNFlag_Ctrl(GETFLAG);
	CDC_FPN_t_SerailFPN.FPN_Status_t.FPN_WRT_ST = CDC_I2CMessage_t.pn_serial_number_status_ctrl.WRT_ST;
	CDC_FPN_t_SerailFPN.FPNDataLength = FPN_SIZE;
	CDC_FPN_t_SerailFPN.pFPNDataBuff = MMIM_pserial_number_Get();
	
	/* excute FPN process */
	FPN_Serial_Control_Status = MFPNC_FPNSerialControl_Process(&CDC_FPN_t_SerailFPN);
    
	switch (FPN_Serial_Control_Status)
	{
    case FPN_WRITE_SUCCESS:
        /* code */
        MMIM_SerialPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteSerialFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_serial_number_status_ctrl.INT_WRT = ENABLE;
        MMIM_pI2CMessage_Get()->pn_serial_number_status_ctrl.INT_WRT = CDC_I2CMessage_t.pn_serial_number_status_ctrl.INT_WRT;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_CHECKSUM_ERR:
        /* code */
        MMIM_SerialPNStatusRegFlag_Ctrl(DISABLE);
        MMIM_WriteSerialFPNFlag_Ctrl(DISABLE);
        CDC_I2CMessage_t.pn_serial_number_status_ctrl.CKSUM_ERR = ENABLE;
        MMIM_pI2CMessage_Get()->pn_serial_number_status_ctrl.CKSUM_ERR = CDC_I2CMessage_t.pn_serial_number_status_ctrl.CKSUM_ERR;
        MMIM_INTBActiveFlag_Ctrl(ENABLE);
        
        break;
    case FPN_WRITE_FAIL:
        /* code */
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        //MMIM_WriteDelAssmFPNFlag_Ctrl(DISABLE);
        
        break;
    case FPN_READ_SUCCESS:
        /* code */
        memcpy(MMIM_pI2CMessage_Get()->serial_number,CDC_FPN_t_SerailFPN.ReadFPNBuff,FPN_SIZE);
        break;
    default:
        break;
	}
#endif
    
	if (TimeOutCnt == (uint8_t)NUM_ZERO)
	{
		TimeOutCnt = (uint8_t)CDC_COUNTER_500; /*reset Cnt*/
	}
	else
	{
		;
	}
    
    CDC_StateMachine_e = CDC_STATE_PROCESS;
    /* End Process */
    
}
/**
* @brief 
* 
*/
static void CDC_Service_Error_Alarm(void)
{ /* Error State, recovery to init state? */
	;
}


void (*const CDC_State_Machine[CDC_STATE_MAX + 1])(void) =
{CDC_Service_Init, CDC_Service_Data_Process, CDC_Service_Error_Alarm};

