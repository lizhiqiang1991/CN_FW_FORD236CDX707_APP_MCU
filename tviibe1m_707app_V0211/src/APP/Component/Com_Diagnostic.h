/**
* @file Com_Diagnostic.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-20
* 
* @copyright Copyright (c) 2020
* 
*/
#ifndef COM_DIAGNOSTIC_H_
#define COM_DIAGNOSTIC_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define CD_EVENT_BUFF_SIZE 3U

#define RECOVEREVENT 2U /* for unlock -> lock recover display */
#define DATABYTE_ONE 0U

#define CD_DCC_EVENT_MSK        0x01U

/* Define Diagnostic event */
typedef enum
{
    eDetectConnetCtrl = 0x01U    ,
}Diagnostic_EVENT_E;

/* Define CD event structure */
typedef struct
{
    /* data */
    Diagnostic_EVENT_E Event_e;
    uint8_t EventBuff[CD_EVENT_BUFF_SIZE];
}CD_EVENT_T;

/* Define CD state machine */
typedef enum
{
	CD_STATE_INIT 		= 0,
	CD_STATE_PROCESS 	= 1,
	CD_STATE_ERROR	 	= 2,
	CD_STATE_MAX 		= CD_STATE_ERROR,
}CD_StateMachine_E;


typedef enum
{
    Detect_IO    = 0,
    Detect_Reg   = 1,
    Report_Err   = 2,
}BL_Diag_State_E;

#define BL_ST_MASK0    0x01
#define BL_ST_MASK1    0x02
#define BL_ST_MASK2    0x04
#define BL_ST_MASK3    0x08

#define DISP_ST_MASK0    0x01
#define DISP_ST_MASK1    0x02
#define DISP_ST_MASK2    0x04
#define DISP_ST_MASK3    0x08

extern void (*const CD_State_Machine[CD_STATE_MAX + 1])(void);

uint8_t  CD_Event_Get(CD_EVENT_T *pEvent_t);
uint8_t  CD_Event_Set(CD_EVENT_T *pEvent_t);
extern CD_StateMachine_E CD_StateMachine_e;

#endif /* COM_DIAGNOSTIC_H_ */