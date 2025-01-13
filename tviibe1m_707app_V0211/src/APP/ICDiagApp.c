
#include "ICDiagApp.h"
#include "I2CMasterApp.h"
#include "MCUDiagApp.h"

#if BACKDOOR_ICDIAG_OPEN
#define	ICDIAG_ICTYPE_I2C			0x01U
#define	ICDIAG_ICTYPE_SPI			0x02U
#define	ICDIAG_ICTYPE_MCU_MEM		0x03U
#define	ICDIAG_ICTYPE_MCU_NVM		0x04U

#define	ICDIAG_STATUS_IDLE			0x00U
#define	ICDIAG_STATUS_PACKED		0x01U
#define	ICDIAG_STATUS_SUBMITTED		0x02U

#define	ICDIAG_FETCH_CTRL_MINLEN	1U
#define	ICDIAG_FETCH_CTRL_MAXLEN	64U

#define	ICDIAG_ICRX_BUFFERSIZE		64U
/*I2C data address : 1 byte*/
/*data : 64 bytes(for NVM page size)*/
#define	ICDIAG_ICTX_BUFFERSIZE		65U

typedef struct
{
	uint8_t u8CommandState;
	uint8_t u8CommandResult;
	uint8_t u8ICType;
	uint8_t u8Channel;
	uint8_t u8DeviceAddr;   /* Slave Addr */
	uint32_t u32DataAddr;   /* Sub Addr   */
	uint8_t u8ICRxLen;
	uint8_t u8ICTxLen;
	uint8_t u8ICRxBuf[ICDIAG_ICRX_BUFFERSIZE];
	uint8_t u8ICTxBuf[ICDIAG_ICTX_BUFFERSIZE];
}ICDIAG_TypeDef;  

static ICDIAG_TypeDef I2CICDiagCtrl={0U};

/******************************************************************************
;	Function Name			:	ICDIAG_FillinDefaultRXData
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
static void ICDIAG_FillinDefaultRXData(void)
{
	uint8_t u8Index=0;

	for(u8Index=0U; u8Index<ICDIAG_ICRX_BUFFERSIZE; u8Index++)
	{
		I2CICDiagCtrl.u8ICRxBuf[u8Index]=0xFFU;
	}	
}

/******************************************************************************
;	Function Name			:	ICDIAG_SetCmdState
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
static void ICDIAG_SetCmdState(uint8_t u8CmdState)
{
	I2CICDiagCtrl.u8CommandState=u8CmdState;
}

/******************************************************************************
;	Function Name			:	ICDIAG_SetCmdResault
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
void ICDIAG_SetCmdResault(uint8_t u8CmdResault)
{
	I2CICDiagCtrl.u8CommandResult=u8CmdResault;
}

/******************************************************************************
;	Function Name			:	ICDIAG_Initialize
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
void ICDIAG_Initialize(void)
{
    (void)memset(&I2CICDiagCtrl, 0U, sizeof(ICDIAG_TypeDef));
	ICDIAG_FillinDefaultRXData();
	ICDIAG_SetCmdState(ICDIAG_STATUS_IDLE);
	ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
}

/******************************************************************************
;	Function Name			:	ICDIAG_GetRxBuffer
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
uint8_t* ICDIAG_GetRxBuffer(void)
{
	return I2CICDiagCtrl.u8ICRxBuf;
}

/******************************************************************************
;	Function Name			:	ICDIAG_CommandCatch
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
void ICDIAG_CmdTrigger(uint8_t u8Cmd, uint8_t u8ICType, uint8_t u8Channel, uint8_t u8DeviceAddr, uint32_t u32DataAddr, uint8_t u8DataLength, uint8_t *u8DataBytes)
{
	int8_t i8Index=0;
	
	/* Fill in command info when state IDLE */
	if(I2CICDiagCtrl.u8CommandState == ICDIAG_STATUS_IDLE)
	{
		/* Check Data Length Mim&Max */
		if((u8DataLength >= ICDIAG_FETCH_CTRL_MINLEN) && (u8DataLength <= ICDIAG_FETCH_CTRL_MAXLEN))
		{
			if(u8Cmd == ICDIAG_CMD_ICCTRL)
			{
				/*Pack I2C write or control package for driver API*/
				if(u8ICType == ICDIAG_ICTYPE_I2C)
				{
					I2CICDiagCtrl.u8ICType=u8ICType;
					I2CICDiagCtrl.u8Channel=u8Channel;
					I2CICDiagCtrl.u8DeviceAddr=u8DeviceAddr;
					I2CICDiagCtrl.u32DataAddr=u32DataAddr;
					/*Bundle buffer size to avoid over flow*/
					if(u8DataLength >= (ICDIAG_ICTX_BUFFERSIZE-1U))
					{
						I2CICDiagCtrl.u8ICTxLen=ICDIAG_ICTX_BUFFERSIZE;
					}
					else
					{
						I2CICDiagCtrl.u8ICTxLen=u8DataLength+1U;
					}
					/*I2C: First byte, data address*/
					I2CICDiagCtrl.u8ICTxBuf[0]=(uint8_t)((uint8_t)u32DataAddr&(uint8_t)0xFFU);
					/*I2C: Data bytes start from second byte*/
					for(i8Index=0; i8Index<((int8_t)u8DataLength); i8Index++)
					{
						I2CICDiagCtrl.u8ICTxBuf[i8Index+1]=*(u8DataBytes+i8Index);
					}
					ICDIAG_FillinDefaultRXData();
					ICDIAG_SetCmdState(ICDIAG_STATUS_PACKED);
					ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
				}
				/*Pack MCU write or control package for driver API*/
				else if((u8ICType == ICDIAG_ICTYPE_MCU_MEM) || (u8ICType == ICDIAG_ICTYPE_MCU_NVM))
				{
					I2CICDiagCtrl.u8ICType=u8ICType;
					I2CICDiagCtrl.u8Channel=u8Channel;
					I2CICDiagCtrl.u8DeviceAddr=u8DeviceAddr;
					I2CICDiagCtrl.u32DataAddr=u32DataAddr;
					I2CICDiagCtrl.u8ICTxLen=u8DataLength;
					/*Bundle buffer size to avoid over flow*/
					if(u8DataLength >= ICDIAG_ICTX_BUFFERSIZE)
					{
						I2CICDiagCtrl.u8ICTxLen=ICDIAG_ICTX_BUFFERSIZE;
					}
					else
					{
						I2CICDiagCtrl.u8ICTxLen=u8DataLength;
					}
					/*MCU: Data bytes start from first byte*/
					for(i8Index=0; i8Index<((int8_t)u8DataLength); i8Index++)
					{
						I2CICDiagCtrl.u8ICTxBuf[i8Index]=*(u8DataBytes+i8Index);
					}
					ICDIAG_FillinDefaultRXData();
					ICDIAG_SetCmdState(ICDIAG_STATUS_PACKED);
					ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
				}
				/*Pack SPI write or control package(TBD!!!)*/
				else
				{/*Nothing*/}
			}
			else if(u8Cmd == ICDIAG_CMD_ICFETCH)
			{
				/*Pack I2C fetch package for driver API*/
				if(u8ICType == ICDIAG_ICTYPE_I2C)
				{
					I2CICDiagCtrl.u8ICType=u8ICType;
					I2CICDiagCtrl.u8Channel=u8Channel;
					I2CICDiagCtrl.u8DeviceAddr=u8DeviceAddr;
					I2CICDiagCtrl.u32DataAddr=u32DataAddr;
					I2CICDiagCtrl.u8ICTxLen=1U;
					I2CICDiagCtrl.u8ICTxBuf[0]=(uint8_t)((uint8_t)u32DataAddr&(uint8_t)0xFFU);
					I2CICDiagCtrl.u8ICRxLen=u8DataLength;
					ICDIAG_FillinDefaultRXData();
					ICDIAG_SetCmdState(ICDIAG_STATUS_PACKED);
					ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
				}
				/*Pack MCU fetch package for driver API*/
				else if((u8ICType == ICDIAG_ICTYPE_MCU_MEM) || (u8ICType == ICDIAG_ICTYPE_MCU_NVM))
				{
					I2CICDiagCtrl.u8ICType=u8ICType;
					I2CICDiagCtrl.u8Channel=u8Channel;
					I2CICDiagCtrl.u8DeviceAddr=u8DeviceAddr;
					I2CICDiagCtrl.u32DataAddr=u32DataAddr;
					I2CICDiagCtrl.u8ICRxLen=u8DataLength;
					ICDIAG_FillinDefaultRXData();
					ICDIAG_SetCmdState(ICDIAG_STATUS_PACKED);
					ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
				}
				/*Pack SPI fetch package(TBD!!!)*/
				else
				{/*Nothing*/}
			}
			else
			{/*Nothing*/}
		}
		else
		{/*Nothing*/}
	}
	else
	{/*Nothing*/}

	(void)i8Index;
}

/******************************************************************************
;	Function Name			:	ICDIAG_Main
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
void ICDIAG_Main(void)
{
	switch(I2CICDiagCtrl.u8CommandState)
	{
		case ICDIAG_STATUS_PACKED:
			/*Use I2C driver API to submit fetch/write package*/
			if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_I2C)
			{
#if ICDIAG_USE_GM_DRV_API
				I2cMasterApp_ICDiag(I2CICDiagCtrl.u8DeviceAddr, I2CICDiagCtrl.u8Channel, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#elif ICDIAG_USE_SGM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD23P6_DRV_API
				I2CM_Diag(I2CICDiagCtrl.u8DeviceAddr, I2CICDiagCtrl.u32DataAddr, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#endif
			}
			/*Use MCU driver API to submit fetch/write package*/
			else if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_MCU_MEM)
			{
#if ICDIAG_USE_GM_DRV_API
				MCUDIAG_MemRW(((uint8_t*)I2CICDiagCtrl.u32DataAddr), I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#elif ICDIAG_USE_SGM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD23P6_DRV_API
				MCUDIAG_MemRW(((uint8_t*)I2CICDiagCtrl.u32DataAddr), I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#endif
			}
			/*Use MCU driver API to submit fetch/write package*/
			else if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_MCU_NVM)
			{
#if ICDIAG_USE_GM_DRV_API
				MCUDIAG_NVMRW(I2CICDiagCtrl.u32DataAddr, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#elif ICDIAG_USE_SGM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD23P6_DRV_API
				MCUDIAG_NVMRW(I2CICDiagCtrl.u32DataAddr, I2CICDiagCtrl.u8ICTxBuf, I2CICDiagCtrl.u8ICTxLen, I2CICDiagCtrl.u8ICRxBuf, I2CICDiagCtrl.u8ICRxLen);
#endif
			}
			/*Use SPI driver API to submit fetch/write package(TBD!!!)*/
			else if(I2CICDiagCtrl.u8ICType == ICDIAG_ICTYPE_SPI)
			{
#if ICDIAG_USE_GM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_SGM_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#elif ICDIAG_USE_FORD23P6_DRV_API
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
#endif
			}
			else
			{
				ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
			}

			ICDIAG_SetCmdState(ICDIAG_STATUS_SUBMITTED);
		break;

		/*Check status after submit, when command complete go back to IDLE*/
		case ICDIAG_STATUS_SUBMITTED:
			if(I2CICDiagCtrl.u8CommandResult == ICDIAG_RESULT_FAIL)
			{
				ICDIAG_FillinDefaultRXData();
				ICDIAG_SetCmdState(ICDIAG_STATUS_IDLE);
			}
			else if(I2CICDiagCtrl.u8CommandResult == ICDIAG_RESULT_SUCCESS)
			{
				ICDIAG_SetCmdState(ICDIAG_STATUS_IDLE);
			}
			else
			{/*Nothing*/}
		break;

		case ICDIAG_STATUS_IDLE:
			ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
		break;

		default:
			ICDIAG_SetCmdState(ICDIAG_STATUS_IDLE);
			ICDIAG_SetCmdResault(ICDIAG_RESULT_NONE);
		break;		  
	}
}

#endif

