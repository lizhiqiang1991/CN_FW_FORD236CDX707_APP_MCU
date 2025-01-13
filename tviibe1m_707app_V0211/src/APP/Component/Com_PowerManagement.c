/**
* @file Com_PowerManagement.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-04-01
* 
* @copyright Copyright (c) 2021
* 
*/

/* include public information */
#include "gPinDef.h"
#include "UartApp.h"
/* include component */
#include "Com_PowerManagement.h"
#include "Com_Communication.h"
/* include module */
#include "Mod_PowerSequence_Ctrl.h"
#include "Mod_DataManagement.h"
/* Include App */
#include "GpioApp.h"


PowerManagement_StateMachine_E sPmState = PM_STATE_INIT;

static Global_PowerState_E CPM_PowerState_e = ePowerState_Stop;
static Global_LockState_E CPM_LockState_e = eUnLocked;


/* For  PowerManagement event  */
CPM_EVENT_T CPM_Event_t = {ePM_Idle};
/* PG_3V3 detect debounce */
// static uint8_t P3V3FaultCnt;
/* PG_5V0 detect debounce */
// static uint8_t P5VFaultCnt;



uint8_t  CPM_Event_Get(CPM_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {  
        memcpy(pEvent_t,&CPM_Event_t,sizeof(CPM_EVENT_T));
        ProcessStatus = true;
    }
    
    return ProcessStatus;
}

uint8_t  CPM_Event_Set(CPM_EVENT_T *pEvent_t)
{
    uint8_t ProcessStatus = NUM_ZERO;
    if(pEvent_t == NULL)
    {
        ProcessStatus = NUM_ZERO;
    }
    else
    {
        memcpy(&CPM_Event_t,pEvent_t,sizeof(CPM_EVENT_T));
        ProcessStatus = true;
    }  
    
    return ProcessStatus;
}




static void CPM_Service_Init(void)
{ 
    /* Init State, init something */
    /* Module initial */
    MPSC_Module_Inital(); // WDT Timer for 10ms  /* this external WDT is no longer use, */
    MPSC_PowerSequence_ON();
    CPM_PowerState_e = ePowerState_Standby;
    CPM_LockState_e = MMIM_LockStatus_Get();
    
    (void)MMIM_PowerState_Ctrl(CPM_PowerState_e);
    
	sPmState = PM_STATE_PROCESS;
}

static void CPM_Event_Process(void)
{   
    CC_EVENT_T CPM_CC_EVENT_t = {eEVENT_DISPEN,{NUM_ZERO}};
    
    CPM_PowerState_e = MMIM_PowerState_Ctrl(ePowerstate_Get);
    
    CPM_LockState_e = MMIM_LockStatus_Get();
    
    (void)CC_Event_Get(&CPM_CC_EVENT_t);
    
    /* Get Shutdown flag */
    if((CPM_CC_EVENT_t.Event_e & eEVENT_HOSTSHUTDOWN) == eEVENT_HOSTSHUTDOWN)
    {   
        CPM_Event_t.Event_e = eEvent_Shutdown; 
        CPM_PowerState_e = MPSC_PowerSequence_Ctrl((MPSC_PowerEvent_E)CPM_Event_t.Event_e, CPM_LockState_e, CPM_PowerState_e);
        //DEBUG_PF("**********power mode=%d**********\r\n",eEVENT_HOSTSHUTDOWN);
        CPM_Event_t.Event_e = ePM_Idle;
        (void)MMIM_PowerState_Ctrl(CPM_PowerState_e);       
        
        /* Clear buffer */ 
        CPM_CC_EVENT_t.Event_e &= (~eEVENT_HOSTSHUTDOWN); 
        (void)CC_Event_Set(&CPM_CC_EVENT_t);
    }
    
    /* Get Jump to boot code flag */    //20211028 Joel
    if((CPM_CC_EVENT_t.Event_e & eEVENT_JUMPTOBOOTcode) == eEVENT_JUMPTOBOOTcode) 
    {
        Cy_SysLib_Delay(20);
		CPM_Event_t.Event_e = eEvent_OFF;
        CPM_PowerState_e = MPSC_PowerSequence_Ctrl((MPSC_PowerEvent_E)CPM_Event_t.Event_e, CPM_LockState_e, CPM_PowerState_e);
        CPM_Event_t.Event_e = ePM_Idle;
        (void)MMIM_PowerState_Ctrl(CPM_PowerState_e); 
        
		CPM_Event_t.Event_e = eEvent_SoftwareReset;
		CPM_PowerState_e = MPSC_PowerSequence_Ctrl((MPSC_PowerEvent_E)CPM_Event_t.Event_e, CPM_LockState_e, CPM_PowerState_e);
        CPM_Event_t.Event_e = ePM_Idle;
        (void)MMIM_PowerState_Ctrl(CPM_PowerState_e); 
        
        /* Clear buffer */ 
        CPM_CC_EVENT_t.Event_e &= (~eEVENT_JUMPTOBOOTcode); 
        (void)CC_Event_Set(&CPM_CC_EVENT_t); 
    }  
    
    if(CPM_Event_t.Event_e != (PowerManagement_EVENT_E)ePM_Idle)
    {
        //DEBUG_PF("**********CPM_Event_t.Event_e=%d**********\r\n",CPM_Event_t.Event_e);
        CPM_PowerState_e = MPSC_PowerSequence_Ctrl((MPSC_PowerEvent_E)CPM_Event_t.Event_e, CPM_LockState_e, CPM_PowerState_e);
        CPM_Event_t.Event_e = ePM_Idle;
        (void)MMIM_PowerState_Ctrl(CPM_PowerState_e);
        
        //DEBUG_PF("**********CPM_PowerState_e=%d**********\r\n",CPM_PowerState_e);
    }
    
}

static void CPM_Service_Data_Process(void)
{   
    CPM_Event_Process();
    
	static uint8_t TimeOutCnt = (uint8_t)CPM_COUNTER_500; /*TimeOut for 500ms*/
	(void)TimeOutCnt;
    
	TimeOutCnt--;
    
	if(TimeOutCnt % NUM_TEN == NUM_ZERO)/* Counter for 20ms timer */
	{	
		/*----------------------------------------*/
		/*Diagnostic P3V3 Fault*/
		/*----------------------------------------*/
        
        
        
		/*----------------------------------------*/
		/*Diagnostic P5V0 Fault*/
		/*----------------------------------------*/
        
        
        
	}
    
	if (TimeOutCnt == (uint8_t)0)
	{
		CPM_PowerState_e = MPSC_PowerState_Get();
		
		/* Get Lock status */
		CPM_LockState_e = MMIM_LockStatus_Get();
        
        
        
        
		TimeOutCnt = (uint8_t)CPM_COUNTER_500; /*reset Cnt*/
	}
	else
	{
		;
	}
    
}


static void CPM_Service_Error_Alarm(void)
{ /* Error State, recovery to init state? */
	;
}


void (*const CPM_State_Machine[PM_STATE_MAX + 1])(void) =
{CPM_Service_Init, CPM_Service_Data_Process, CPM_Service_Error_Alarm};