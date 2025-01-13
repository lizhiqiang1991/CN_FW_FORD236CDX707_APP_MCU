/*
* IF_DetectGPIO.h
*
*  Created on: 2020�~11��13��
*      Author: DerekWen
*/

#ifndef IF_DETECTGPIO_H_
#define IF_DETECTGPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "gPinDef.h"
/* Define requirted interface */
typedef uint8_t (*MDGPIO_IF_POWERSTATE_CTRL_T)(uint8_t u8FlagState);
typedef uint8_t (*MDGPIO_IF_LLOSS_SET_T)(uint8_t u8LLOSSStatus);
typedef uint8_t (*MDGPIO_IF_INTBACTIVEFLAG_SET_T)(uint8_t u8FlagState);
typedef Global_LockState_E (*MDGPIO_IF_LOCKSTATE_CTRL_T)(Global_LockState_E Status_e);

typedef struct 
{
    /* data */
    MDGPIO_IF_POWERSTATE_CTRL_T    MDGPIO_PwrInitState_Ctrl;
    MDGPIO_IF_LLOSS_SET_T   MDGPIO_LLOSS_Set;
    MDGPIO_IF_INTBACTIVEFLAG_SET_T      MDGPIO_INTBActiveFlag_Set;
    MDGPIO_IF_LOCKSTATE_CTRL_T  MDGPIO_LockState_Ctrl;
}MDGPIO_IF_DETECTGPIO_T;


#endif /* IF_DETECTGPIO_H_ */
