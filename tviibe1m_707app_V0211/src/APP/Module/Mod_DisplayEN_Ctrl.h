/*
* Mod_DisplayEN_Ctrl.h
*
*  Created on: 2020�~11��10��
*      Author: DerekWen
*/

#ifndef MOD_DISPLAYEN_CTRL_H_
#define MOD_DISPLAYEN_CTRL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "gPinDef.h"

#define Display_Enabled 1U
#define Display_Disable 0U


uint8_t MDEN_DispENProcess_Ctrl(uint8_t ucDispEN,Global_PowerState_E ePowerState,Global_LockState_E eLockState);
void MDEN_SourceDriver_DispOn_CheckReset(void);
void MDEN_TConBistMode_En(uint8_t TConBistMode_Ctrl);
void MDEN_SourceDriver_DispOn_CheckReset(void);
void MDEN_SourceDriver_DispOn_CheckReset(void);
#endif /* MOD_DISPLAYEN_CTRL_H_ */
