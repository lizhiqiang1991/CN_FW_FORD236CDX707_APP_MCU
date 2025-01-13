/**
* @file Com_BatteryProtect.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-26
* 
* @copyright Copyright (c) 2020
* 
*/

#ifndef COM_BATTERYPROTECT_H_
#define COM_BATTERYPROTECT_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define CBP_EVENT_BUFF_SIZE 		3U

/* Define CBP event */
// typedef enum
// {
//     eExampleEvent = 0U

// }CBP_EVENT_E;

/* Define Event structure */
typedef struct
{
    /* data */
    // CBP_EVENT_E CBP_Event_e;
    uint8_t EventBuff[CBP_EVENT_BUFF_SIZE];
    
}CBP_EVENT_T;

/* Define TASK state machine */
typedef enum
{
	CBP_STATE_INIT 		= 0,
	CBP_STATE_PROCESS 	= 1,
	CBP_STATE_ERROR	 	= 2,
	CBP_STATE_MAX 		= CBP_STATE_ERROR,
}CBP_StateMachine_E;

uint8_t  CBP_Event_Get(CBP_EVENT_T *pEvent_t);
uint8_t  CBP_Event_Set(CBP_EVENT_T *pEvent_t);
extern void (*const CBP_State_Machine[CBP_STATE_MAX + 1])(void);
extern CBP_StateMachine_E CBP_StateMachine_e;






#endif