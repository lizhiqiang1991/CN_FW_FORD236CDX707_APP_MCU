/**
* @file Com_Communication.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-03-30
* 
* @copyright Copyright (c) 2021
* 
*/
#ifndef COM_COMMUNICATION_H_
#define COM_COMMUNICATION_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define EVENT_BUFF_SIZE 45U

#define CC_MEM_SHIFT4 4U
#define CC_MEM_SHIFT5 5U
#define CC_MEM_SHIFT6 6U
#define CC_MEM_SHIFT8 8U
#define CC_MEM_SHIFT12 12U
#define CC_MEM_SHIFT16 16U
#define CC_MEM_SHIFT20 20U

#define CC_EVENT_BUFF_SIZE_4 4U
#define CC_EVENT_BUFF_SIZE_25 25U

typedef enum
{
	CC_STATE_INIT 		= 0,
	CC_STATE_PROCESS 	= 1,
	CC_STATE_ERROR	 	= 2,
	CC_STATE_MAX 		= CC_STATE_ERROR,
}CC_StateMachine_E;


typedef enum
{
	eEVENT_DISPEN = 0x01U,
	eEVENT_BLEN =0x02U	,
	eEVENT_SCANDIRECT	=0x04U,
	eEVENT_HOSTSHUTDOWN	=0x08U,
	eEVENT_JUMPTOBOOTcode	=0x10U,
	eEVENT_BISTMODE	=0x20U,
	eEVENT_DIAGNOSTICMESSAGE =0x40U,
}Communication_EVENT_E;

typedef struct
{
	uint8_t Event_e;
	uint8_t EventBuff[EVENT_BUFF_SIZE];
}CC_EVENT_T;


uint8_t  CC_Event_Get(CC_EVENT_T *pEvent_t);
uint8_t  CC_Event_Set(CC_EVENT_T *pEvent_t);
extern void (*const CC_State_Machine[CC_STATE_MAX + 1])(void);
extern CC_StateMachine_E CC_StateMachine_e;

#endif /* COM_COMMUCATION_H_ */