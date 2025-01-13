/**
* @file Com_TemperatureProtect.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-19
* 
* @copyright Copyright (c) 2020
* 
*/

/* include public information */
#include "gPinDef.h"
#include "UartApp.h"
/* include conponent */
#include "Com_DeviceControl.h"
#include "Com_TemperatureProtect.h"
/* include module */

#include "Mod_TemperatureProtection_Ctrl.h"
#include "Mod_DataManagement.h"
/* include app */
#include "AdcApp.h"

static void CTP_Service_Init(void);
static void CTP_Service_Data_Process(void);
static void CTP_Service_Error_Alarm(void);


/* Create CTP state machine */
void (*const CTP_State_Machine[CTP_STATE_MAX + 1])(void) =
{CTP_Service_Init, CTP_Service_Data_Process, CTP_Service_Error_Alarm};

CTP_StateMachine_E CTP_StateMachine_e = CTP_STATE_INIT;

CTP_EVENT_T  CTP_Event_t;

uint32_t Mod_TemProtect_result;

static uint16_t u16BacklightNtcAdcValue_L;
static uint16_t u16BacklightNtcAdcValue_R;

static uint16_t u16Pcb984NtcAdcValue;
static uint16_t u16MCURef_1V_ADCValues;
static uint16_t u16BacklightUserDuty_Get;

static I2C_MESSAGES_T CTP_I2cMessage_t;
/*for CTP_Service_Data_Process */
static uint8_t TimeOutCnt = (uint8_t)CTP_COUNTER_500; /*TimeOut for 500ms*/
static uint16_t Mod_TemProtect_result_PWM = NUM_ZERO;
static uint8_t  Mod_TemProtect_result_status = NUM_ZERO;
static uint8_t  last_Mod_TemProtect_result_status = NUM_ZERO;

static uint16_t Mod_TemProtect_result_PWM_temp = 0xFFFFU;

int16_t i16Temperature_BL = 0;
int16_t i16Temperature_984 = 0;
Mod_TemPro_Ctrl_TableTypedef CTP_NTCTemp_t = {NUM_ZERO};

uint8_t  CTP_Event_Get(CTP_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {  
        memcpy(pEvent_t,&CTP_Event_t,sizeof(CTP_EVENT_T));
        ProcessStatus = true;
    }
    
    return ProcessStatus;
}

uint8_t  CTP_Event_Set(CTP_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {
        memcpy(&CTP_Event_t,pEvent_t,sizeof(CTP_EVENT_T));
        ProcessStatus = true;
    }  
    
    return ProcessStatus;
}


/* Start Program */
static void CTP_Service_Init(void)
{ 
    /* Init State, init something */
    /* Module initial */
	
    memset(CTP_Event_t.EventBuff,NUM_ZERO,sizeof(CTP_Event_t.EventBuff));
    
    CTP_StateMachine_e = CTP_STATE_PROCESS;
}

static void CTP_Service_Data_Process(void)
{
	(void)TimeOutCnt;
	(void)Mod_TemProtect_result_status;
	(void)last_Mod_TemProtect_result_status;
    
    uint16_t u16BacklightNtcAdcValue_L_temp;
    uint16_t u16BacklightNtcAdcValue_R_temp;
    uint16_t u16Pcb984NtcAdcValue_temp;
    
	CDC_EVENT_T CTP_CDC_EVENT_t;
	uint16_t u16SetNtcAdcValue = NUM_ZERO;
    
	TimeOutCnt--;
	
	CTP_I2cMessage_t = *MMIM_pI2CMessage_Get();
 	
	if (TimeOutCnt == (uint8_t)0U)
	{
		
		/* Get NTC Temperature */
		CTP_NTCTemp_t = Mod_TemPro_Ctrl_Table_Get();
		if(CTP_NTCTemp_t.DataSource == TEMPSOUREFROM_BL_R || CTP_NTCTemp_t.DataSource == TEMPSOUREFROM_BL_L)
		{
			MMIM_BLTemperatureVal_Set(CTP_NTCTemp_t.symbol,CTP_NTCTemp_t.Temperature_Value,CTP_NTCTemp_t.DataSource);
		}
		else if(CTP_NTCTemp_t.DataSource == TEMPSOUREFROM_984)
		{
			MMIM_PCBTemperatureVal_Set(CTP_NTCTemp_t.symbol,CTP_NTCTemp_t.Temperature_Value,CTP_NTCTemp_t.DataSource);
		}
		else
		{
			;
		}
		
		u16MCURef_1V_ADCValues = AdcApp_Ref_1V_Get();
        
		u16BacklightUserDuty_Get &= REGISTER_CLEAR_16BIT;
		u16BacklightUserDuty_Get |= CTP_I2cMessage_t.lcd_backlight_pwm_value.BL_PWM_7_0;
		u16BacklightUserDuty_Get |= CTP_I2cMessage_t.lcd_backlight_pwm_value.BL_PWM_9_8 << SHIFT_BIT_8;
        
        /* Get PCB NTC ADC value */
        u16BacklightNtcAdcValue_L_temp = AdcApp_BacklightNtcAdcVaules_L_Get();
        u16BacklightNtcAdcValue_R_temp = AdcApp_BacklightNtcAdcVaules_R_Get();
        u16Pcb984NtcAdcValue_temp = AdcApp_984TEMPAdcValue_Get();
        
        u16BacklightNtcAdcValue_L = MTPC_BacklightNtcAdcValue_Average_API_L(u16BacklightNtcAdcValue_L_temp);
        u16BacklightNtcAdcValue_R = MTPC_BacklightNtcAdcValue_Average_API_R(u16BacklightNtcAdcValue_R_temp);
        u16Pcb984NtcAdcValue = MTPC_PCBNtcAdcValue_Average_API(u16Pcb984NtcAdcValue_temp);
        
		/* Set value to Data management */
		if(u16BacklightNtcAdcValue_L >= u16BacklightNtcAdcValue_R)
		{	
			i16Temperature_BL = MTPC_Temperature_Get(SOUREFROM_BL_L, u16BacklightNtcAdcValue_L,u16MCURef_1V_ADCValues);
			i16Temperature_984 = MTPC_Temperature_Get(SOUREFROM_984, u16Pcb984NtcAdcValue,u16MCURef_1V_ADCValues);
			//DEBUG_PF("Temperature_984(BL_L>BL_R) = %d\r\n",i16Temperature_984);
			if(i16Temperature_BL >= i16Temperature_984 )
			{
				u16SetNtcAdcValue = u16BacklightNtcAdcValue_L;
				Mod_TemProtect_result=MTPC_TemperatureProtection_Ctrl(SOUREFROM_BL_L, u16SetNtcAdcValue,u16MCURef_1V_ADCValues,u16BacklightUserDuty_Get);
			}
			else
			{
				u16SetNtcAdcValue = u16Pcb984NtcAdcValue;
				Mod_TemProtect_result=MTPC_TemperatureProtection_Ctrl(SOUREFROM_984, u16SetNtcAdcValue,u16MCURef_1V_ADCValues,u16BacklightUserDuty_Get);
				//DEBUG_PF("TemProtect_result(BL_L>BL_R) = %d\r\n",Mod_TemProtect_result);
			}			
		}
		else
		{
			i16Temperature_BL = MTPC_Temperature_Get(SOUREFROM_BL_R, u16BacklightNtcAdcValue_R,u16MCURef_1V_ADCValues);
			i16Temperature_984 = MTPC_Temperature_Get(SOUREFROM_984, u16Pcb984NtcAdcValue,u16MCURef_1V_ADCValues);
			//DEBUG_PF("Temperature_984 = %d\r\n",i16Temperature_984);
			if(i16Temperature_BL >= i16Temperature_984 )
			{
				u16SetNtcAdcValue = u16BacklightNtcAdcValue_R;
				Mod_TemProtect_result=MTPC_TemperatureProtection_Ctrl(SOUREFROM_BL_R, u16SetNtcAdcValue,u16MCURef_1V_ADCValues,u16BacklightUserDuty_Get);
			}
			else
			{
				u16SetNtcAdcValue = u16Pcb984NtcAdcValue;
				Mod_TemProtect_result=MTPC_TemperatureProtection_Ctrl(SOUREFROM_984, u16SetNtcAdcValue,u16MCURef_1V_ADCValues,u16BacklightUserDuty_Get);
				//DEBUG_PF("TemProtect_result = %d\r\n",Mod_TemProtect_result);
			}			
		}
        
		MMIM_PCBNTCADC_Set(u16Pcb984NtcAdcValue);		
		Mod_TemProtect_result_status=(Mod_TemProtect_result>>16U)&0xFFU;
		Mod_TemProtect_result_PWM=Mod_TemProtect_result & 0xFFFFU;
		//DEBUG_PF("Mod_TemProtect_result_PWM = %d\r\n",Mod_TemProtect_result_PWM);  
                //if(0)
		if(MMIM_DeratingFlag_Get() == ENABLE)
		{
			//DEBUG_PF("Derating enable = %d\r\n",Mod_TemProtect_result_status);
			if ((Mod_TemProtect_result_status == derating_mode) || (Mod_TemProtect_result_status == shutdown_mode))
			{				
				/* Set TEERR */
				MMIM_DisplayStatusReg_Ctrl(SOURENOTFROMI2C,eTemperatureERR,LEVEL_HIGH);
                
				/* Send event to derating */
				if((memcmp(&Mod_TemProtect_result_PWM, &Mod_TemProtect_result_PWM_temp, sizeof(Mod_TemProtect_result_PWM))) != NUM_ZERO || (Mod_TemProtect_result_PWM_temp < MMIM_PWMTargetValue_Ctrl(GETPWMFLAG)))
				{
					Mod_TemProtect_result_PWM_temp=Mod_TemProtect_result_PWM;
					
					if(u16BacklightUserDuty_Get >= Mod_TemProtect_result_PWM)
					{
						if(CDC_Event_Get(&CTP_CDC_EVENT_t) == true)
						{
							CTP_CDC_EVENT_t.Event_e |= eBLBrightnessCtrl;
							CTP_CDC_EVENT_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_ONE] = (Mod_TemProtect_result_PWM>>8U) & 0x03U;
							CTP_CDC_EVENT_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_TWO] = (Mod_TemProtect_result_PWM & 0xffU);
							CTP_CDC_EVENT_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_THR] = ENABLE;
							CDC_Event_Set(&CTP_CDC_EVENT_t);
						}
					}
                    
				}
				else
				{
					;
				}
			}
			else if(Mod_TemProtect_result_status == normal_mode)
			{
				if ((last_Mod_TemProtect_result_status == derating_mode) || (last_Mod_TemProtect_result_status == shutdown_mode))
				{
					if(CDC_Event_Get(&CTP_CDC_EVENT_t) == true)
					{
						CTP_CDC_EVENT_t.Event_e |= eBLBrightnessCtrl;
						CTP_CDC_EVENT_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_ONE] = (u16BacklightUserDuty_Get>>8U) & 0x03U;
						CTP_CDC_EVENT_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_TWO] = (u16BacklightUserDuty_Get & 0xffU);
						CTP_CDC_EVENT_t.EventBuff[CDC_MEM_SHIFT6 + CDC_BUFFBYTE_THR] = ENABLE;
						CDC_Event_Set(&CTP_CDC_EVENT_t);
					}
				}
			}
			else
			{
				;
			}
			last_Mod_TemProtect_result_status = Mod_TemProtect_result_status;
		}
		
		TimeOutCnt = (uint8_t)CTP_COUNTER_500; /*reset Cnt*/
	}
	else
	{
		;
	}
}

static void CTP_Service_Error_Alarm(void)
{ /* Error State, recovery to init state? */
	;
}


