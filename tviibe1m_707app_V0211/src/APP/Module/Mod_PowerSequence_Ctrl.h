/*
* Mod_PowerSequence_Ctrl.h
*
*  Created on: 2020�~11��10��
*      Author: DerekWen
*/

#ifndef MOD_POWERSEQUENCE_CTRL_H_
#define MOD_POWERSEQUENCE_CTRL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "gPinDef.h"



// #define WATCHDOGCOUNT 1U

// #define PowerSequenceIdele 0U
// #define PowerSequenceOnGoing 1U

// #define INSTANCE_WATCHDOG 0U
// #define INSTANCE_ACCDETECT 1U
// #define	THRESHOLD_SYS_PWR_ON			40 // 1/* count in 1ms timer for 40ms  */
// #define SLEEPOUTDELAY 2U
// #define PG_RECOVER_DEBOUNCE_COUNT 500U

typedef enum
{
    ePowerOFF = 0U,
    ePowerON,
    ePowerShutdown,
    eWakeMAX20419Fault,
    eAbnormalSequence,
    eAbnormalSequence_3V3,
    eAbnormalSequence_5V0,
    eSoftwareReset
        
}MPSC_PowerEvent_E;

Global_PowerState_E MPSC_PowerState_Get(void);
Global_PowerState_E MPSC_PowerSequence_ON(void);
Global_PowerState_E MPSC_PowerSequence_OFF(Global_PowerState_E ePowerState);
void MPSC_Module_Inital(void);
Global_PowerState_E MPSC_PowerSequence_Ctrl(MPSC_PowerEvent_E ePowerEvent, Global_LockState_E eLockState,Global_PowerState_E ePowerState);
uint8_t MPSC_Check_Power_Alive(uint8_t status, uint8_t times);

#endif /* MOD_POWERSEQUENCE_CTRL_H_ */
