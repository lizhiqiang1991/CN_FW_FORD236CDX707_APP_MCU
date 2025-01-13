
#include "MCUDiagApp.h"
//#include "Memory_Pool.h"
//#include "app/inc/EepromApp.h"
//#include "hal/inc/RegsAccessDriver.h"
//#include "main.h"

#if BACKDOOR_ICDIAG_OPEN
#include "FlashApp.h"
#include "hal_fmc.h"
#define EEPROM_WRITE_TIMEOUT    (uint32_t)100000000U
//workflash access define
#define USING_SMALLSECTORWORK_FLASH             1
#define USING_LARGESECTORWORK_FLASH             0

//static uint32_t gu32MCUDiagWriteBuff[64];
#if USING_SMALLSECTORWORK_FLASH
#define EEPROM_SMALLSECTOR_SIZE             128UL
static uint8_t u8FlashBuff[EEPROM_SMALLSECTOR_SIZE];
#endif

#if USING_LARGESECTORWORK_FLASH
#define EEPROM_LARGESECTOR_SIZE             2048UL
static uint8_t u8LSFlashBuff[EEPROM_LARGESECTOR_SIZE];
#endif
/******************************************************************************
;	Function Name			:	MCUDIAG_MemRW
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
void MCUDIAG_MemRW(uint8_t *u8DataAddr, uint8_t *u8TxData, uint8_t u8TxLen, uint8_t *u8RxData, uint8_t u8RxLen)
{
	uint8_t u8Index=0U;
	if(u8RxLen > 0U)
	{
		for(u8Index=0U; u8Index<u8RxLen; u8Index++)
		{
			*(u8RxData+u8Index)=*(u8DataAddr+u8Index);
		}
	}
	else
	{/*Nothing*/}

	if(u8TxLen > 0U)
	{
		for(u8Index=0U; u8Index<u8TxLen; u8Index++)
		{
			*(u8DataAddr+u8Index)=*(u8TxData+u8Index);
		}
	}
	else
	{/*Nothing*/}

	ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);

	(void)u8Index;
}

/******************************************************************************
;	Function Name			:	MCUDIAG_NVMRW
;	Function Description	:	
;	Parameters				:	
;	Return Values			:	
******************************************************************************/
void MCUDIAG_NVMRW(uint32_t u32DataAddr, uint8_t *u8TxData, uint8_t u8TxLen, uint8_t *u8RxData, uint8_t u8RxLen)
{


	uint16_t u16Index = 0U;
//	bool bStatus = true;
//	uint32_t u32TimeOutCnt = 0U;

	if(u8RxLen > 0U)
	{

            memcpy(u8RxData,(uint8_t *)u32DataAddr,u8RxLen);

	}
	else
	{/*Nothing*/}

	if(u8TxLen > 0U)
	{
//            __disable_irq();
#if USING_SMALLSECTORWORK_FLASH
          if (u32DataAddr >= ADDR_WorkFlash_S0 && u32DataAddr <= ADDR_WorkFlash_S127){
              if ((u8TxLen % SIZE_WORK_FLASH_SSECTOR_BYTE) > 0 )
                  u8TxLen = (u8TxLen / SIZE_WORK_FLASH_SSECTOR_BYTE + 1) * SIZE_WORK_FLASH_SSECTOR_BYTE;
              
              (void)memset(u8FlashBuff, 0xFFU, sizeof(u8FlashBuff));
              memcpy(u8FlashBuff,u8TxData,u8TxLen);
              (void)Flash_Work_Sector_Write(u32DataAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,u8FlashBuff, (sizeof(u8FlashBuff)/sizeof(uint8_t)));
          }
#endif
#if USING_LARGESECTORWORK_FLASH
          else if (u32DataAddr >= ADDR_WorkFlash_L0 && u32DataAddr <= ADDR_WorkFlash_L0){
              if ((u8TxLen % SIZE_WORK_FLASH_LSECTOR_BYTE) > 0 )
                  u8TxLen = (u8TxLen / SIZE_WORK_FLASH_LSECTOR_BYTE + 1) * SIZE_WORK_FLASH_LSECTOR_BYTE;
              
              (void)memset(u8LSFlashBuff, 0xFFU, sizeof(u8LSFlashBuff));
              memcpy(u8LSFlashBuff,u8TxData,u8TxLen);
              (void)Flash_Work_Sector_Write(u32DataAddr, SIZE_WORK_FLASH_LSECTOR_BYTE,u8LSFlashBuff, (sizeof(u8LSFlashBuff)/sizeof(uint8_t)));
          }
#endif
          else /*Not inside workflash*/
              return;
	  


	}
	else
	{/*Nothing*/}

	ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);

	(void)u16Index;
}

#endif

