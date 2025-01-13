/**
* @file Com_DeviceControl.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-18
* 
* @copyright Copyright (c) 2020
* 
*/
#ifndef COM_DEVICECONTROL_H_
#define COM_DEVICECONTROL_H_


#include <stdint.h>
#include <stdbool.h>
#include <string.h>



#define CDC_EVENT_BUFF_SIZE 32U
#define CDC_COUNTER_500 500U
#define CDC_BUFFBYTE_ONE 0U
#define CDC_BUFFBYTE_TWO 1U
#define CDC_BUFFBYTE_THR 2U

#define DISPNORMALDATA 0x03U

#define CDC_DISPEN_EVENT_MSK        0x01U
#define CDC_BLEN_EVENT_MSK          0x02U
#define CDC_SCANDIRECT_EVENT_MSK    0x04U
#define CDC_HOSTSHUTDOWN_EVENT_MSK  0x08U
#define CDC_BISTMOSE_EVENT_MSK      0x20U
#define CDC_DIAGNOSTICMESSAGE_EVENT_MSK      0x40U

#define CC_MEM_SHIFT5 5U
#define CC_MEM_SHIFT6 6U

#define CDC_MEM_SHIFT3  3U
#define CDC_MEM_SHIFT6  6U
#define CDC_MEM_SHIFT9  9U
#define CDC_MEM_SHIFT12 12U
#define CDC_MEM_SHIFT15 15U
#define CDC_MEM_SHIFT21 18U



typedef enum
{
    eDispOffSeq = 0x00,
    eTouchResetSeq = 0x01,
    eDispAbnormal = 0x02,
    eDispOnSeq
}CDC_DispSeq_E;

typedef enum
{
    /* data */
    eDisplayEN = 0x01U,
    eBacklightEN = 0x02U,
    eBLBrightnessCtrl = 0x04U,
    eTSCCtrl = 0x08U,
    eScanDirCrl = 0x10U,
    eDisplayEN_Sequence = 0x20U,
    eFPN_Ctrl = 0x40U,
}DeviceControl_EVENT_E;

typedef enum
{
	CDC_STATE_INIT 		= 0,
	CDC_STATE_PROCESS 	= 1,
	CDC_STATE_ERROR	 	= 2,
	CDC_STATE_MAX 		= CDC_STATE_ERROR,
}CDC_StateMachine_E;

typedef struct 
{
    DeviceControl_EVENT_E Event_e;
    uint8_t EventBuff[CDC_EVENT_BUFF_SIZE];
}CDC_EVENT_T;



extern void (*const CDC_State_Machine[CDC_STATE_MAX + 1])(void);

uint8_t  CDC_Event_Get(CDC_EVENT_T *pEvent_t);
uint8_t  CDC_Event_Set(CDC_EVENT_T *pEvent_t);
extern CDC_StateMachine_E CDC_StateMachine_e;

#endif /* COM_DEVICECONTROL_H_ */
