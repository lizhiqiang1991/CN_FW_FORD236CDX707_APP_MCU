/**
* @file Com_PowerManagement.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-04-01
* 
* @copyright Copyright (c) 2021
* 
*/
#ifndef COM_POWERMANAGEMENT_H_
#define COM_POWERMANAGEMENT_H_


#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define CPM_COUNTER_500 500U
#define DISPSHUTDOWNENABLE ENABLE



typedef enum
{
	eEvent_OFF = 0U,
    eEvent_ON,
    eEvent_Shutdown,
    eEvent_PMICFault,
    eEvent_AbnormalVoltage,
	eEvent_AbnormalVoltage_3V3,
	eEvent_AbnormalVoltage_5V0,
	eEvent_SoftwareReset,
	ePM_Idle
        
}PowerManagement_EVENT_E;

typedef struct
{
	/* data */
	PowerManagement_EVENT_E Event_e;
}CPM_EVENT_T;

typedef enum
{
	PM_STATE_INIT 		= 0,
	PM_STATE_PROCESS 	= 1,
	PM_STATE_ERROR	 	= 2,
	PM_STATE_MAX 		= PM_STATE_ERROR,
}PowerManagement_StateMachine_E;


/*  */

extern void (*const CPM_State_Machine[PM_STATE_MAX + 1])(void);

uint8_t  CPM_Event_Get(CPM_EVENT_T *pEvent_t);
uint8_t  CPM_Event_Set(CPM_EVENT_T *pEvent_t);


extern PowerManagement_StateMachine_E sPmState;
#endif /* COM_POWERMANAGEMENT_H_ */
