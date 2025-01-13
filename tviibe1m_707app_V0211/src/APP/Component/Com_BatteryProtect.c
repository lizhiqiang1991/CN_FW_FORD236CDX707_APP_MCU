
/* include public information */
#include "gPinDef.h"
#include "UartApp.h"
/* include component */
#include "Com_PowerManagement.h"
#include "Com_BatteryProtect.h"
#include "Com_Diagnostic.h"
/* include module */
#include "Mod_BatteryProtection_Ctrl.h"
#include "Mod_DataManagement.h"

/* include App */
#include "AdcApp.h"


static CBP_EVENT_T CBP_Event_t;

/*state machine */

CBP_StateMachine_E CBP_StateMachine_e = CBP_STATE_INIT;

/* declare global value*/
static Global_PowerState_E CBP_PowerState_e = ePowerState_Stop;
// static Global_LockState_E CBP_LuckState_e = eLocked;
static uint16_t u16BatterAdcVal = NUM_ZERO;
static uint16_t u16MCU1V_ADCValues = NUM_ZERO;
static Battery_process_TypeDef tdBatteryProcessStatus = BAT_initial_mode;
static uint8_t ServiceTimeOutCnt = (uint8_t)CPM_COUNTER_500; /*TimeOut for 500ms*/
static uint8_t PMICFaultSendEventFlag = DISABLE;

static uint8_t u8VoltCount = 0;
/* Start Program */
/**
* @brief IQR send queue callbcak function
* 
*/


uint8_t  CBP_Event_Get(CBP_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {
        memcpy(pEvent_t,&CBP_Event_t,sizeof(CBP_EVENT_T));
        ProcessStatus = true;
    }
    
    return ProcessStatus;
}
uint8_t  CBP_Event_Set(CBP_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {
        memcpy(&CBP_Event_t,pEvent_t,sizeof(CBP_EVENT_T));
        ProcessStatus = true;
    }
    
    return ProcessStatus;
}

void CBP_CPMEventPMICFaultFromISR_Send(void)
{
	CPM_EVENT_T CBP_CPM_Event_t;
	CD_EVENT_T CBP_CD_Event_t;	
	memset(CBP_CD_Event_t.EventBuff,NUM_ZERO,sizeof(CBP_CD_Event_t.EventBuff));
	
	/* send event to disable detecting disconnection */
	CBP_CD_Event_t.Event_e = eDetectConnetCtrl;
	CBP_CD_Event_t.EventBuff[NUM_ZERO] = DISABLE;
	(void)CD_Event_Set(&CBP_CD_Event_t);
	
	if(CPM_Event_Get(&CBP_CPM_Event_t) == true)
	{
		CBP_CPM_Event_t.Event_e = eEvent_PMICFault;
		CPM_Event_Set(&CBP_CPM_Event_t);
		//DEBUG_PF("********CPM_Event_Set=e%d************\r\n",CBP_CPM_Event_t.Event_e);
	}
    
}

void CBP_CPMEventPMICRecover_Send(void)
{
	CPM_EVENT_T CBP_CPM_Event_t;
	CD_EVENT_T CBP_CD_Event_t;	
	memset(CBP_CD_Event_t.EventBuff,NUM_ZERO,sizeof(CBP_CD_Event_t.EventBuff));
	
	if(CPM_Event_Get(&CBP_CPM_Event_t) == true)
	{
		CBP_CPM_Event_t.Event_e = eEvent_ON;
		CPM_Event_Set(&CBP_CPM_Event_t);
	}
	
	/* send event to enable detecting disconnection */
	CBP_CD_Event_t.Event_e = eDetectConnetCtrl;
	CBP_CD_Event_t.EventBuff[NUM_ZERO] = ENABLE;
	(void)CD_Event_Set(&CBP_CD_Event_t);
}

static void CBP_Service_Data_Process(void)
{
	
	(void)ServiceTimeOutCnt;
    
	ServiceTimeOutCnt--;
    
	CBP_PowerState_e = MMIM_PowerState_Ctrl(ePowerstate_Get);
	
	if(CBP_PowerState_e == ePowerState_AbnormalRun)
	{
		u16BatterAdcVal = AdcApp_BatteryVolAdcValue_Get();
		u16MCU1V_ADCValues = AdcApp_Ref_1V_Get();		
		if( MBPC_PMICRecover_Detect( ENABLE,u16BatterAdcVal,u16MCU1V_ADCValues) == ENABLE)  /* recover voltage  */
		{
			//CBP_CPMEventPMICRecover_Send();  	Leo mark this line 2022_01_06		

			//CBP_PowerState_e = ePowerState_Standby;
			//MMIM_PowerState_Ctrl(CBP_PowerState_e);

			MMIM_PowerState_Ctrl(ePowerState_Standby);
		}
	}
    
	if (ServiceTimeOutCnt == (uint8_t)0)
	{
		u16BatterAdcVal = AdcApp_BatteryVolAdcValue_Get();
		//DEBUG_PF("Get Battery Vol Adc Value=%d\r\n",u16BatterAdcVal);
		u16MCU1V_ADCValues = AdcApp_Ref_1V_Get();
		//DEBUG_PF("Get MCU 1V_ADC Value=%d\r\n",u16MCU1V_ADCValues);
		tdBatteryProcessStatus = MBPC_BatteryProtection_Ctrl( u16BatterAdcVal,u16MCU1V_ADCValues);
        //DEBUG_PF("Get battery mode=%d\r\n",tdBatteryProcessStatus);
		
		/* send battery voltage from Module to Mod_Datamanagement */
		MMIM_BattVolVal_Set(MBPC_BattPro_BattVolt_Get());
        
#if 1		
        /* leo 20211125 */
        if(MMIM_VoltageDRT_Flag_Get() == false)
        {
            if(MBPC_BattPro_BattVolt_Get() <= 95) /* if battery voltage lower or equal than 9.5 V */
            {
                u8VoltCount++;   /* this count will add per 500 ms */
				 //DEBUG_PF("battery Vol=%d\r\n",MBPC_BattPro_BattVolt_Get());
            }
            else
            {
                u8VoltCount = 0;
            }
            
            if(u8VoltCount == 20)
            {
                MMIM_VoltageDRT_Flag_Set(true);  /* set voltage derating flag */
				//DEBUG_PF("******lo_Vol trigger drating******\r\n");
                u8VoltCount = 0;
            }
        }
#endif               
        
		if(( tdBatteryProcessStatus == BAT_highvoltage_mode) || (tdBatteryProcessStatus == BAT_lowvoltage_mode))
		{
			if(tdBatteryProcessStatus == BAT_highvoltage_mode)
			{
				MMIM_Client_Diag_Set(General_Byte_1 , (MMIM_Client_Diag_Get(General_Byte_1) | Battery_Vol_High));
			}
			else if(tdBatteryProcessStatus == BAT_lowvoltage_mode)
			{
				MMIM_Client_Diag_Set(General_Byte_1 , (MMIM_Client_Diag_Get(General_Byte_1) | Battery_Vol_Low));
			}
			else
			{
				;
			}
            //DEBUG_PF("******RST_RQ = High *******\r\n");
			if(PMICFaultSendEventFlag == DISABLE)
			{
				CPM_EVENT_T CBP_CPM_Event_t;
                
				if(CPM_Event_Get(&CBP_CPM_Event_t) == true)
				{
					//CBP_CPM_Event_t.Event_e = eEvent_PMICFault;
					CBP_CPM_Event_t.Event_e = eEvent_Shutdown;
					(void)CPM_Event_Set(&CBP_CPM_Event_t);				
                    
					PMICFaultSendEventFlag = ENABLE;
				}
			}
		}
		else
		{
			PMICFaultSendEventFlag = DISABLE;
		}
        
		ServiceTimeOutCnt = (uint8_t)CPM_COUNTER_500; /*reset Cnt*/
	}
	else if(ServiceTimeOutCnt % NUM_FIFTY == NUM_ZERO)/* Counter for 50ms timer */
	{	
		u16BatterAdcVal = AdcApp_BatteryVolAdcValue_Get();
		u16MCU1V_ADCValues = AdcApp_Ref_1V_Get();
		(void)MBPC_BatteryProtection_Ctrl( u16BatterAdcVal,u16MCU1V_ADCValues);		
	}
	else
	{
		;
	}	
}

static void CBP_Service_Init(void)
{ 
        /* Init State, init something */
	/* Module initial */
	//(void)HAL_GPIO_PinWrite(PIN_U301_EN_VBATT_SENSE,PIN_HIGH); /*Enable battery voltage sensor*/
    memset(CBP_Event_t.EventBuff,NUM_ZERO,sizeof(CBP_Event_t.EventBuff));
	MBPC_BatteryProtection_Function_Initial(CBP_CPMEventPMICFaultFromISR_Send);
	//DEBUG_PF("********CBP_Service_Init************\r\n");
	CBP_StateMachine_e = CBP_STATE_PROCESS;
}

static void CBP_Service_Error_Alarm (void)
{
    ;
}
void (*const CBP_State_Machine[CBP_STATE_MAX + 1])(void) =
{CBP_Service_Init, CBP_Service_Data_Process, CBP_Service_Error_Alarm};



