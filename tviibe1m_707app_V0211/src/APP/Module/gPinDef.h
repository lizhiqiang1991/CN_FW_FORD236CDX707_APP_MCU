/*
* gPinDef.h
*
*  Created on: 2020�~11��10��
*      Author: DerekWen
*/

#ifndef GPINDEF_H_
#define GPINDEF_H_


#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define COM_TCH_CTRL_ID  1U
#define RESET_REQ_CTRL_ID  1U
#define COM_Diagnostic_ID 3U
#define Mod_DetectGPIO_ID 4U

typedef enum
{
    ePowerState_Stop = 0U,
    ePowerState_AbnormalRun,
    ePowerState_AbnormalRun_3V3,
    ePowerState_AbnormalRun_5V0,
    ePowerState_Recover,
    ePowerState_HostShutdown,
    ePowerState_LockShutdown,
    ePowerState_Standby,
    ePowerState_Normal,
    ePowerState_TPReset,
    ePowerState_Sleep,
    ePowerState_Sleep_OnGoing,
    ePowerstate_Get
}Global_PowerState_E;

typedef enum
{
	eLocked,
    eUnLocked
}Global_LockState_E;

/* Define Shift bit */
#define		SHIFT_BIT_0			0U
#define		SHIFT_BIT_1			1U
#define		SHIFT_BIT_2			2U
#define		SHIFT_BIT_3			3U
#define		SHIFT_BIT_4			4U
#define		SHIFT_BIT_5			5U
#define		SHIFT_BIT_6			6U
#define		SHIFT_BIT_7			7U
#define		SHIFT_BIT_8			8U
#define		SHIFT_BIT_9			9U
#define		SHIFT_BIT_10		10U
#define		SHIFT_BIT_11		11U
#define		SHIFT_BIT_12		12U
#define		SHIFT_BIT_13		13U
#define		SHIFT_BIT_14		14U
#define		SHIFT_BIT_15		15U
#define		SHIFT_BIT_16		16U
#define		SHIFT_BIT_17		17U
#define		SHIFT_BIT_18		18U
#define		SHIFT_BIT_19		19U
#define		SHIFT_BIT_20		20U
#define		SHIFT_BIT_21		21U
#define		SHIFT_BIT_22		22U
#define		SHIFT_BIT_23		23U
#define		SHIFT_BIT_24		24U
#define		SHIFT_BIT_25		25U
#define		SHIFT_BIT_26		26U
#define		SHIFT_BIT_27		27U
#define		SHIFT_BIT_28		28U
#define		SHIFT_BIT_29		29U
#define		SHIFT_BIT_30		30U
#define		SHIFT_BIT_32		32U

#define		SHIFT_BIT_31		31U
//---------------------------- Function Switch Module  ----------------------------//
#define CLOCK0_DEBUG_MSG         DISABLE

//---------------------------- Delcare Content  ----------------------------//
#define     NUM_ZERO            0U
#define     NUM_ONE             1U
#define     NUM_FOUR            4U
#define     NUM_TEN             10U
#define     NUM_FIFTY           50U

#define     DISABLE            NUM_ZERO
#define     ENABLE             NUM_ONE

#define		INPUT				0U
#define		OUTPUT				1U

#define		LEVEL_HIGH			1U
#define		LEVEL_LOW			0U

#define		BIT0				0U
#define		BIT1				1U
#define		BIT2				2U
#define		BIT3				3U
#define		BIT4				4U
#define		BIT5				5U
#define		BIT6				6U
#define		BIT7				7U
#define		BIT8				8U
#define		BIT9				9U
#define		BIT10				10U
#define		BIT11				11U
#define		BIT12				12U
#define		BIT13				13U
#define		BIT14				14U
#define		BIT15				15U
#define		BIT16				16U
#define		BIT17				17U
//---------------------------- I/O Pin Definition ---------------------------//




//---------------------------- Declare Constant ------------------------------//
#define	PANEL_PWR_ON		1
#define	PANEL_PWR_OFF		0

#define	SYS_PWR_ON			1U
#define	SYS_PWR_OFF			0U

#define	TCON_FAULT_SET		0x01
#define	PMIC_FAULT_SET		0x02

#define	TCON_FAULT_CLR		0xFE
#define	PMIC_FAULT_CLR		0xFD




#define AbnormalLowVol 3U
#define AbnormalLowVol_PG 4U

#define LM61460FaultDeounceTime 2000    /*count 2000 in 1ms*/

#define SLA_SIZE_TWO     2
#define SLA_SIZE_ONE     1

#define ErrorNum 16U

#define NOTHING 0U

#define GETFLAG 99U
#define GETPWMFLAG 9999U



#endif /* GPINDEF_H_ */
