/*
* Mod_DetectGPIO.h
*
*  Created on: 2020�~11��12��
*      Author: DerekWen
*/

#ifndef MOD_DETECTGPIO_H_
#define MOD_DETECTGPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "IF_DetectGPIO.h"


#define LCDFAULTDEBOUNCECOUNT 5                 /* 20 -> 5, 5*25 = 125 ms */
#define LCD_BL_FAULTDEBOUNCECOUNT 5             /* 20 -> 5, 5*25 = 125 ms */
#define TCON_ASIL_DEBOUNCECOUNT 5               /* 20 -> 5, 5*25 = 125 ms */
#define LLOSSFAULTDEBOUNCECOUNT 4
#define LLOSSFAULTDEBOUNCECOUNT_RECOVER 50
#define DISCONNECTERRDEBOUNCECOUNT 20


typedef enum
{
    /* data */
    eNormal = NUM_ZERO,
    eFault
}FaultState_E;


typedef enum
{
	FPCOut = 0,
	FPC_Read,
}FPCStrategyState_E;

void MDGPIO_PIN984Lock_IRQCallback_Instal (void);
void MDGPIO_PortD_IRQCallback_Instal (void);
void MDGPIO_Interface_Set(MDGPIO_IF_POWERSTATE_CTRL_T pPowerState_Ctrl,
                          MDGPIO_IF_LLOSS_SET_T pLLOSS_Set,
                          MDGPIO_IF_INTBACTIVEFLAG_SET_T pINTB_Set,
                          MDGPIO_IF_LOCKSTATE_CTRL_T pLockState_ctrl);
//void MDGPIO_DisplayENState_Set(uint8_t u8DisplayENState);
FaultState_E MDGPIO_Display_Diagnostic(void);
FaultState_E Diagnostic_LCD_BL_Fault(void);
FaultState_E MDGPIO_LLOSS_Detect(void);
uint8_t MDGPIO_TouchStatus_Detect (void);
FaultState_E MDGPIO_DisconnectERR_L_Detect(void);
FaultState_E MDGPIO_DisconnectERR_R_Detect(void);
FaultState_E MDGPIO_Diagnostic_LCD_BL_Fault(void);
FaultState_E MDGPIO_Diagnostic_TCON_Fault(void);
FaultState_E MDGPIO_Diagnostic_TFT_Bias_Fault(void);

#endif /* MOD_DETECTGPIO_H_ */
