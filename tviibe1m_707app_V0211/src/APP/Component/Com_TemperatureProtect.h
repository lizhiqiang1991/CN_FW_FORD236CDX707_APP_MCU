/**
* @file Com_TemperatureProtect.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-19
* 
* @copyright Copyright (c) 2020
* 
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CTP_EVENT_BUFF_SIZE 3U
#define CTP_COUNTER_500 20U    /* change from 500 to 20 */
#define REGISTER_CLEAR_16BIT 0x0000

/* Define TempProt event */
//typedef enum
//{
//    eExampleEvent = 0U
//
//}TempProt_EVENT_E;

/* Define CTP event structure */
typedef struct
{
    /* data */
    //    TempProt_EVENT_E Event_e;
    uint8_t EventBuff[CTP_EVENT_BUFF_SIZE];
}CTP_EVENT_T;


/* Define TASK state machine */
typedef enum
{
	CTP_STATE_INIT 		= 0,
	CTP_STATE_PROCESS 	= 1,
	CTP_STATE_ERROR	 	= 2,
	CTP_STATE_MAX 		= CTP_STATE_ERROR,
}CTP_StateMachine_E;


extern void (*const CTP_State_Machine[CTP_STATE_MAX + 1])(void);

uint8_t  CTP_Event_Get(CTP_EVENT_T *pEvent_t);
uint8_t  CTP_Event_Set(CTP_EVENT_T *pEvent_t);
extern CTP_StateMachine_E CTP_StateMachine_e;
