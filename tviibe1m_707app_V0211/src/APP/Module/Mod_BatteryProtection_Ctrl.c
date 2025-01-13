/*---------------------------- Include File ---------------------------------*/
/* include global */
#include "gPinDef.h"

/* include module */
#include "Mod_BatteryProtection_Ctrl.h"
#include "Mod_DataManagement.h"

/* Include App */
#include "GpioApp.h"
#include "TimerApp.h"
#include "AdcApp.h"


#define RDevider1 100U  /*R329*/
#define RDevider2 681U	/*R321*/
#define Battery_factor 10U	

#define ON  							((uint8_t)1U)
#define OFF 							((uint8_t)0U)
#define Reset_Counter 					((uint16_t)0U)

#define IN_Off                         ((uint8_t)1U)
#define IN_On                          ((uint8_t)2U)

#define DebounceHigh2Low_cnt           ((uint16_t)20U)
#define DebounceLow2High_cnt           ((uint16_t)20U)

uint16_t Voltage_ActiveLow = 55U;  // North America 6V=60 ,China 7V=70  -> 6V to 5.5V , leo -> 55 to 90
uint16_t Voltage_ActiveHigh = 185U; // 18V to 18.5V

uint16_t Recover_Voltage_ActiveLow = 60U;  // North America 6V=60 ,China 7V=70 , leo -> 60 -> 95
uint16_t Recover_Voltage_ActiveHigh = 180U; // 18V

static uint8_t u8ShutdownStatus = DISABLE;
static uint8_t u8WakeupStatus = DISABLE;

static uint8_t u8DebounceResault = IN_Off;
// static uint16_t MAX20419FaultDeounceCnt = NUM_ZERO;
static uint16_t LM61460FaultDeounceCnt = NUM_ZERO;

static MBP_CPMEVENT_SEND MBP_CPM_ShutdownEvent_Send = NULL;
// static MBP_CPMEVENT_SEND MBP_CPM_RecoverEvent_Send = NULL;

/*PinIO_DebounceResult*/
static uint8_t u8PinIO_DebounceResult_initial=DISABLE;
static uint8_t u8PinIO_DebounceResult_status=IN_On;
static uint16_t u16Cnt=Reset_Counter;
/*MBPC_BatteryVoltAdcValues_Average*/
static uint16_t u16BatteryVoltAdcValues_Delay1 = 0U;
static uint16_t u16BatteryVoltAdcValues_Delay2[3]={0U};
/*MBPC_BatteryProtection_Process*/
static uint8_t u8BatteryProtection_initial=0U;
static Battery_process_TypeDef u8BatteryProtection_status = BAT_initial_mode;

static uint16_t gu16BattVoltage;
/* 16141 fault pin detect ,send */
uint8_t u8SendEventStatus = DISABLE;

/*------------------------ Declare External Var -----------------------------*/
uint8_t PinIO_DebounceResult(uint8_t PinIOStatus,uint16_t High2Low_cnt,uint16_t Low2High_cnt)
{
	uint8_t u8PinIOStatus_out;
    
	if (u8PinIO_DebounceResult_initial==DISABLE)
	{
		u8PinIO_DebounceResult_initial=ENABLE;
		u8PinIO_DebounceResult_status=IN_On;/*In Off state machine*/
		u8PinIOStatus_out=ON;
		u16Cnt=Reset_Counter;
	}
	else if (u8PinIO_DebounceResult_status==IN_Off)
	{
		u8PinIOStatus_out=OFF;
		if (u16Cnt>= Low2High_cnt)
		{
			u8PinIO_DebounceResult_status=IN_On;/*In On state machine*/
			u8PinIOStatus_out=ON;
			u16Cnt=Reset_Counter;			
		}
		else if (PinIOStatus==1U)
		{
			u16Cnt=u16Cnt+1U;	
		}
		else
		{
			u16Cnt=0U;
		}
	}
	else
	{
        /* case IN_On: */
		u8PinIOStatus_out=ON;
		if (u16Cnt>= High2Low_cnt)
		{
			u8PinIO_DebounceResult_status = IN_Off;/*In Off state machine*/
			u8PinIOStatus_out=OFF;
			u16Cnt = Reset_Counter;			
		}
		else if (PinIOStatus==0U)
		{
			u16Cnt=u16Cnt+1U;	
		}
		else
		{
			u16Cnt=0U;
		}
	}
	return u8PinIOStatus_out;
}
/******************************************************************************
;       Function Name			:	
;       Function Description	:	
;       Parameters				:	
;       Return Values			:	
;		Source ID				:	SWE3_xxx_xx
******************************************************************************/   
uint16_t MBPC_BatteryVoltAdcValues_Average(uint16_t u16BatteryVoltAdcValues)
{
	uint16_t u16BatteryVoltAdcValues_Average;
    
#if 0
	u16BatteryVoltAdcValues_Average=(u16BatteryVoltAdcValues+u16BatteryVoltAdcValues_Delay1+u16BatteryVoltAdcValues_Delay2[0])/3;
	
	u16BatteryVoltAdcValues_Delay1 = u16BatteryVoltAdcValues;
	u16BatteryVoltAdcValues_Delay2[0] =u16BatteryVoltAdcValues_Delay2[1];
	u16BatteryVoltAdcValues_Delay2[1] = u16BatteryVoltAdcValues;
#else
	
	u16BatteryVoltAdcValues_Average=(u16BatteryVoltAdcValues+u16BatteryVoltAdcValues_Delay1+u16BatteryVoltAdcValues_Delay2[0]+u16BatteryVoltAdcValues_Delay2[1])>>2;
    
	u16BatteryVoltAdcValues_Delay1 = u16BatteryVoltAdcValues;
	u16BatteryVoltAdcValues_Delay2[0] = u16BatteryVoltAdcValues_Delay2[1];
	u16BatteryVoltAdcValues_Delay2[1] = u16BatteryVoltAdcValues_Delay2[2];
	u16BatteryVoltAdcValues_Delay2[2] = u16BatteryVoltAdcValues;
#endif
	return u16BatteryVoltAdcValues_Average;
}

Battery_process_TypeDef MBPC_BatteryProtection_Process(uint16_t Battery_Voltage,uint16_t u16Voltage_ActiveLow,uint16_t u16Voltage_ActiveHigh,uint8_t pin61460Falt_status)
{
    
	if (u8BatteryProtection_initial==DISABLE)
	{
		u8BatteryProtection_initial=ENABLE;
		u8BatteryProtection_status=BAT_initial_mode;
	}
	else
	{
		if(pin61460Falt_status == true)
		{
			if (Battery_Voltage <= u16Voltage_ActiveHigh && Battery_Voltage >= u16Voltage_ActiveLow)
			{
				u8BatteryProtection_status = BAT_normal_mode;	
			}
			else if(Battery_Voltage > u16Voltage_ActiveHigh)
			{
				u8BatteryProtection_status = BAT_highvoltage_mode;	
			}
			else if(Battery_Voltage < u16Voltage_ActiveLow)
			{
				u8BatteryProtection_status = BAT_lowvoltage_mode;
			}
			else
			{
				;
			}
		}
		else 
		{
			if (Battery_Voltage > u16Voltage_ActiveHigh)
			{
				u8BatteryProtection_status=BAT_highvoltage_mode;
			}
			else if (Battery_Voltage < u16Voltage_ActiveLow)
			{
				u8BatteryProtection_status = BAT_lowvoltage_mode;
			}
			else
			{
				u8BatteryProtection_status = BAT_lowvoltage_mode;	
			}
		}
	}	
	return u8BatteryProtection_status;
}

void MBPC_BatteryProtection_61460FaultInIRQ_Detect(void* context)
{
    UNUSED (context);
	static uint8_t u8GPIOStatus = NUM_ZERO;
    
	(void)HAL_GPIO_PinRead(PIN_LM61460_PG,&u8GPIOStatus);  //3.6V , PIN_LM61460_PG input =0V
	
	u8DebounceResault = PinIO_DebounceResult(u8GPIOStatus,DebounceHigh2Low_cnt,DebounceLow2High_cnt);
    
	if (u8DebounceResault == OFF)
	{
		MMIM_Client_Diag_Set(General_Byte_0 , (MMIM_Client_Diag_Get(General_Byte_0) | P3V3_PG_Fail));
		if(u8SendEventStatus == DISABLE)
		{
            
			/* Send Event to CPM to shutdown */
			if (MBP_CPM_ShutdownEvent_Send != NULL)
			{
				MBP_CPM_ShutdownEvent_Send();		
				u8SendEventStatus = ENABLE;
			}
            
		}
	}
	else
	{
		u8SendEventStatus = DISABLE;
	}
	
}

uint8_t MBP_PMIC_Debounce (uint16_t DebounceCnt)
{
	uint8_t DebouceResult = false;
    
	uint8_t u8GPIOStatus = NUM_ZERO;
	(void)HAL_GPIO_PinRead(PIN_LM61460_PG, &u8GPIOStatus);
    
	if(u8GPIOStatus == LEVEL_HIGH)
	{
		LM61460FaultDeounceCnt++;
	}
	else
	{
		LM61460FaultDeounceCnt = 0u;
	}
    
	/*count 500 in 1ms*/
	if(LM61460FaultDeounceCnt >= DebounceCnt)
	{
		DebouceResult = true;
		/*20200511*/
		LM61460FaultDeounceCnt = 0u;
	}
    
	return DebouceResult;
}

uint16_t MBPC_BatteryProtection_RECOVER_Ctrl(uint16_t u16BatteryVoltAdcValues, uint16_t u16MCU1V_ADCValues)
{
	uint16_t u16Battery_Voltage = NUM_ZERO;
	
    u16Battery_Voltage=(u16BatteryVoltAdcValues*(RDevider1 + RDevider2)*Battery_factor)/(RDevider1*u16MCU1V_ADCValues);	//Voltage divider rule
    gu16BattVoltage = u16Battery_Voltage;/*For DQA test*/
    
	return u16Battery_Voltage;
    
}
/*---------------------------- Start Program ---------------------------------*/
#if 0
uint8_t MBPC_PMICRecover_Detect(uint8_t u8ActiveFlag)
{
	uint8_t u8ProcessStatus = NUM_ZERO;
    
	if(u8ActiveFlag == ENABLE)
	{
		
		if(MBP_PMIC_Debounce(MAX20419FaultDeounceTime) == true)
		{
			
			u8ProcessStatus =NUM_ONE;
			
		}
		else
		{
			u8ProcessStatus = DISABLE;
		}
	}
	else
	{
		u8ProcessStatus = DISABLE;
	}
	
    
	return u8ProcessStatus;
}
#else
uint8_t MBPC_PMICRecover_Detect(uint8_t u8ActiveFlag, uint16_t u16BatteryVoltAdcValues, uint16_t u16MCU1V_ADCValues)
{
	uint8_t u8ProcessStatus = NUM_ZERO;
	uint16_t u16Battery_Voltage = NUM_ZERO;
    
	if(u8ActiveFlag == ENABLE)
	{
		u16Battery_Voltage = MBPC_BatteryProtection_RECOVER_Ctrl(u16BatteryVoltAdcValues, u16MCU1V_ADCValues);
		if(MBP_PMIC_Debounce(LM61460FaultDeounceTime) == true) //2 sec
		{   
			if(u16Battery_Voltage >= Recover_Voltage_ActiveLow && u16Battery_Voltage <= Recover_Voltage_ActiveHigh)
			{
				
				u8ProcessStatus = NUM_ONE;
			}
			else
			{
				u8ProcessStatus = DISABLE;
			}			
		}
		else
		{
			u8ProcessStatus = DISABLE;
		}
	}
	else
	{
		u8ProcessStatus = DISABLE;
	}
	
    
	return u8ProcessStatus;
}
#endif

uint8_t MBPC_BatteryProtection_ShutdownStatus_Set(uint8_t u8Stauts)
{
	u8ShutdownStatus = u8Stauts;
    
	return u8ShutdownStatus;
}

uint8_t MBPC_BatteryProtection_WakeupStatus_Ctrl(uint8_t u8Stauts)
{
	switch (u8Stauts)
	{
	case GETFLAG/* constant-expression */:
		/* code */
		break;
        
	default:
        
		u8WakeupStatus = u8Stauts;
        
		break;
	}
    
    
	return u8WakeupStatus;
}

void MBPC_100usTimerProcess_Install(void)
{
    /* Create PMIC_DET Timer for 1ms */
    HAL_Timer_Init(PMIC_DET_TIMER_1MINSEC,&gts_1mSec_Timer_Continuous_Config);  //1ms
    
    /*Timer register callback*/
    HAL_Timer_Callback_Register (PMIC_DET_TIMER_1MINSEC,MBPC_BatteryProtection_61460FaultInIRQ_Detect,NULL);
    
    /*Timer Enable*/
    HAL_Timer_Active(PMIC_DET_TIMER_1MINSEC  ,FUNC_ENABLE);
    
    /* PMIC_DET TIMER_2MINSEC timer start*/ 
    HAL_Timer_Start(PMIC_DET_TIMER_1MINSEC   ,FUNC_ENABLE);
    
    /*Enable PMIC_DET TIMER_2MINSEC interrupt & recounting */
    HAL_Timer_Interrupt_Set (PMIC_DET_TIMER_1MINSEC, FUNC_ENABLE, CYHAL_TCPWM_IRQ_PRIORITY);    
}

uint8_t MBPC_BatteryProtection_Function_Initial(MBP_CPMEVENT_SEND pShutdownEventFun)
{
	/* Enable Battery Sensor */
    (void)HAL_GPIO_PinWrite(PIN_U301_EN_VBATT_SENSE,PIN_HIGH); /*Enable battery voltage sensor*/
    
	/* Install 100us IRQ Callback */
	MBPC_100usTimerProcess_Install();
    
	/* Install Send Event Callback */
	MBP_CPM_ShutdownEvent_Send = pShutdownEventFun;
	
	return true;
 	
}
/******************************************************************************
;       Function Name			:	
;       Function Description	:	

;       Parameters				:	
;       Return Values			:	
;		Source ID				:	
******************************************************************************/
Battery_process_TypeDef MBPC_BatteryProtection_Ctrl(uint16_t u16BatteryVoltAdcValues, uint16_t u16MCU1V_ADCValues)
{
	uint16_t u16BatteryVoltage_ADCAverage_temp=0U;
	uint16_t u16Battery_Voltage=0U;
	uint8_t u8PinIO_DebounceStatus = NUM_ONE;
	Battery_process_TypeDef BatteryProtection_Ctrl_result;
	
	u16BatteryVoltage_ADCAverage_temp = MBPC_BatteryVoltAdcValues_Average(u16BatteryVoltAdcValues);
	MMIM_BattVolADCVal_Set(u16BatteryVoltage_ADCAverage_temp); 
        u16Battery_Voltage=(u16BatteryVoltage_ADCAverage_temp*(RDevider1 + RDevider2)*Battery_factor)/(RDevider1*u16MCU1V_ADCValues);	//Voltage divider rule
        gu16BattVoltage=u16Battery_Voltage;/*For DQA test*/
        u8PinIO_DebounceStatus = u8DebounceResault;
    
	BatteryProtection_Ctrl_result = MBPC_BatteryProtection_Process(u16Battery_Voltage,Voltage_ActiveLow,Voltage_ActiveHigh,u8PinIO_DebounceStatus);
    
	return BatteryProtection_Ctrl_result;
    
}

uint16_t MBPC_BattPro_BattVolt_Get(void)
{
    return gu16BattVoltage;
}


