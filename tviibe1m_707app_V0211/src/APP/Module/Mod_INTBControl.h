/**
* @file Mod_INTBControl.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-26
* 
* @copyright Copyright (c) 2020
* 
*/

#ifndef MOD_INTBCONTROL_H_
#define MOD_INTBCONTROL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef uint8_t (*MINTBC_INTBFLAG_CTRL)(uint8_t Flagstatus);

typedef enum
{
	Wait10ms = 0,
	WaitInterupt,
	AssertINBIN,
	KeepWait
}StrategyState_E;

void MINTBC_2msTimerProcess_Install(MINTBC_INTBFLAG_CTRL pINTB_Ctrl_fun);
Global_LockState_E MINTBC_LockPinStatus_Ctrl(Global_LockState_E u8Status);

#endif