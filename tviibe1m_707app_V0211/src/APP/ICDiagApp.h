
#ifndef ICDIAGAPP_H    /* Guard against multiple inclusion */
#define ICDIAGAPP_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "Ford_Config.h"

//#define BACKDOOR_ICDIAG_OPEN			1U
#ifdef BACKDOOR_ICDIAG_OPEN

#define ICDIAG_USE_GM_DRV_API			0U
#define ICDIAG_USE_SGM_DRV_API			0U
#define ICDIAG_USE_FORD23P6_DRV_API		1U

#if ICDIAG_USE_GM_DRV_API
#include "app/inc/I2cMasterApp.h"
#include "app/inc/MCUDiagApp.h"
#endif

#if ICDIAG_USE_FORD23P6_DRV_API
#include "I2CMasterApp.h"
#endif
#if ICDIAG_USE_GM
#define	ICDIAG_CMD_ICFETCH				0xAAU
#define	ICDIAG_CMD_READ					0xABU  /* read  */
#define	ICDIAG_CMD_ICCTRL				0xACU  /* write */

#elif ICDIAG_USE_FORD23P6_DRV_API
#define	ICDIAG_CMD_ICFETCH				0xD0U
#define	ICDIAG_CMD_READ					0xD1U  /* read  */
#define	ICDIAG_CMD_ICCTRL				0xD2U  /* write */

#define ICDIAG_BUFFERSIZE                               64U
#endif
#define	ICDIAG_RESULT_NONE				0x00U
#define	ICDIAG_RESULT_SUCCESS		    0x01U
#define	ICDIAG_RESULT_FAIL				0x02U

extern void ICDIAG_Initialize(void);
extern void ICDIAG_SetCmdResault(uint8_t u8CmdResault);
extern uint8_t* ICDIAG_GetRxBuffer(void);
extern void ICDIAG_CmdTrigger(uint8_t u8Cmd, uint8_t u8ICType, uint8_t u8Channel, uint8_t u8DeviceAddr, uint32_t u32DataAddr, uint8_t u8DataLength, uint8_t *u8DataBytes);
extern void ICDIAG_Main(void);

#endif
#endif

