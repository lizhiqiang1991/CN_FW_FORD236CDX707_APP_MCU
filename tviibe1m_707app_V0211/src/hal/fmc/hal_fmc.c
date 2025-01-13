/***************************************************************************
*\file hal_fmc.c
****************************************************************************/ 

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_fmc_def.h"
#include "hal_fmc.h"
                                      
#define CODE_FLASH_ROW_DATA_SIZE_512         512   /*(512 Bytes = 4096 bits)*/
#define CODE_FLASH_ROW_DATA_SIZE_32          32    /*(32  Bytes = 256 bits)*/
#define CODE_FLASH_ROW_DATA_SIZE_8            8    /*(8   Bytes = 64 bits)*/
#define WORK_FLASH_ROW_DATA_SIZE              4    /*(4   Bytes = 32 bits)*/
#define CY_FLASH_DRV_MODE                CY_FLASH_DRIVER_BLOCKING 

static bool FMC_Blank_Verify(uint32_t u32SectorAddr ,uint32_t u32SectorSizeInWord)
{
    bool bPass = true;
    uint32_t* p_u32TestFlsTop = (uint32_t*)u32SectorAddr;
 
    for(uint32_t u32WordId = 0; u32WordId < u32SectorSizeInWord; u32WordId++)
    {
        if (p_u32TestFlsTop[u32WordId] != 0xFFFFFFFF)
        {
            bPass = false;
            break;
        }
    }
    return bPass;
}

static uint8_t FMC_WorkFlash_Blank_Check(uint32_t u32SectorAddr)
{
    uint8_t u8HalResult;
    uint32_t u32SectorSizeInWord;
    if (true == Cy_Flash_IsWorkSmallSector(u32SectorAddr))
        u32SectorSizeInWord = CY_WORK_SES_SIZE_IN_WORD;
    else
        u32SectorSizeInWord = CY_WORK_LES_SIZE_IN_WORD; 
    
    cy_stc_flash_blankcheck_config_t blankCheckConfig;
    blankCheckConfig.addrToBeChecked       = (uint32_t*)u32SectorAddr;
    blankCheckConfig.numOfWordsToBeChecked = u32SectorSizeInWord;
    
    if (CY_FLASH_DRV_SUCCESS == Cy_Flash_BlankCheck(NULL, &blankCheckConfig, CY_FLASH_DRIVER_BLOCKING))
        u8HalResult = DRIVER_TRUE;
    else
        u8HalResult = DRIVER_FALSE;
    
    return u8HalResult;
}

uint8_t HAL_FMC_Initial(fmc_config_t*  tsConfig)
{
    uint8_t u8HalResult;
    if ((NULL != tsConfig) && (tsConfig->u8ReadWaitStates <= FLASH_WS_MAX))
    {
        /*********** Setting wait state for FLASH **********/
        FLASHC->unFLASH_CTL.stcField.u4MAIN_WS = tsConfig->u8ReadWaitStates;
        /*  Flash Write Enable   */
        Cy_Flashc_MainWriteEnable();
        Cy_Flashc_WorkWriteEnable();
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t Hal_FMC_Deinit(void)
{
     Cy_Flashc_MainWriteDisable();
     Cy_Flashc_WorkWriteDisable();
     return DRIVER_TRUE;
}

uint8_t HAL_FMC_Status_Get(uint32_t *pu32Status)
{
    uint8_t u8HalResult;
    if (NULL != pu32Status)
    {
        *pu32Status = (uint32_t)Cy_Flash_GetDrvStatus(NULL);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;  
}

uint8_t HAL_FMC_Flash_Safety(uint8_t u8Enable)
{
    uint8_t u8HalResult;
    
    if ((FUNC_ENABLE == u8Enable) || (FUNC_DISABLE == u8Enable))
    {
        if (FUNC_ENABLE == u8Enable)
            Cy_Flashc_MainWriteDisable();
        else
            Cy_Flashc_MainWriteEnable();
        
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_FMC_FlashEraseAll(uint8_t u8Verify)
{
    uint8_t u8HalResult;
    if(CY_FLASH_DRV_SUCCESS == Cy_Flash_EraseAll(NULL, CY_FLASH_DRV_MODE) &&
       ((FUNC_ENABLE == u8Verify) || (FUNC_DISABLE == u8Verify)))
    {
        if (FUNC_ENABLE == u8Verify)
        { 
            /*Check large & small sector*/
            if (true == FMC_Blank_Verify(CY_FLASH_LG_SBM_TOP,(CY_FLASH_LG_SBM_SIZE/4)))
            {
                if (true ==  FMC_Blank_Verify(CY_FLASH_SM_SBM_TOP,(CY_FLASH_SM_SBM_SIZE/4)))
                    u8HalResult = DRIVER_TRUE;
                else
                    u8HalResult = DRIVER_FALSE;
            }
            else
            {
                u8HalResult = DRIVER_FALSE;
            }
        }
        else
        {
            u8HalResult = DRIVER_TRUE;
        }
    }
    else
    {
         u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    
    return u8HalResult;
}

uint8_t HAL_FMC_FlashEraseSector(uint32_t u32SectorAddr ,uint8_t u8Verify)
{
    uint8_t u8HalResult;
    cy_en_flashdrv_status_t eStatus;
    cy_stc_flash_erasesector_config_t eraseSectorConfig = {0};
    
     if(CY_FLASH_IN_BOUNDS == Cy_Flash_MainBoundsCheck(u32SectorAddr) &&
       ((FUNC_ENABLE == u8Verify) || (FUNC_DISABLE == u8Verify)))
     {
         /* Erase code flash sector and verify */
         eraseSectorConfig.blocking = CY_FLASH_ERASESECTOR_BLOCKING;
         eraseSectorConfig.intrMask = CY_FLASH_ERASESECTOR_NOT_SET_INTR_MASK;
         eraseSectorConfig.Addr = (uint32_t*)u32SectorAddr;
         eStatus = Cy_Flash_EraseSector(NULL, &eraseSectorConfig, CY_FLASH_DRV_MODE);
         
         if (CY_FLASH_DRV_SUCCESS == eStatus)
         {
             uint32_t u32SectorSizeInWord;
             if (FUNC_ENABLE == u8Verify)
             {
                 if (true == Cy_Flash_IsMainSmallSector(u32SectorAddr))
                     u32SectorSizeInWord = CY_CODE_SES_SIZE_IN_WORD;
                 else
                     u32SectorSizeInWord = CY_CODE_LES_SIZE_IN_WORD; 
                   
                 if (true == FMC_Blank_Verify(u32SectorAddr,u32SectorSizeInWord))
                     u8HalResult = DRIVER_TRUE;
                 else
                     u8HalResult = DRIVER_FALSE;
             }
             else
                u8HalResult = DRIVER_TRUE;
         }
         else
         {
             u8HalResult = DRIVER_FALSE;
         }
     }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
 
    return u8HalResult;
}

uint8_t HAL_FMC_FlashWrite (uint8_t *pu8Data,uint32_t u32Address,uint16_t  u16DataLength)
{
    uint8_t u8HalResult;
    cy_en_flashdrv_status_t eStatus;
    uint32_t u32ProgError = 0u;
    uint8_t  u8LastRow[CODE_FLASH_ROW_DATA_SIZE_8] = {0u};
    cy_stc_flash_programrow_config_t programRowConfig = {0u};
    programRowConfig.blocking = CY_FLASH_PROGRAMROW_BLOCKING;
    programRowConfig.skipBC   = CY_FLASH_PROGRAMROW_SKIP_BLANK_CHECK;
    programRowConfig.dataLoc  = CY_FLASH_PROGRAMROW_DATA_LOCATION_SRAM;
    programRowConfig.intrMask = CY_FLASH_PROGRAMROW_NOT_SET_INTR_MASK;
    programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_4096BIT;
    
    if ((NULL != pu8Data)   &&\
        (u16DataLength > 0u) &&\
        (FLASHC_FM_CTL_ECT->unMAIN_FLASH_SAFETY.stcField.u1MAINFLASHWRITEENABLE == 1u) &&\
        (Cy_Flash_MainBoundsCheck(u32Address) == CY_FLASH_IN_BOUNDS))
    {
        uint8_t* pData = pu8Data; 
        uint32_t u32Remaining = (uint32_t) u16DataLength; 
        uint32_t u32FlashAddr =  u32Address;  
        
        for (; u32Remaining >= CODE_FLASH_ROW_DATA_SIZE_512 ; u32Remaining-=CODE_FLASH_ROW_DATA_SIZE_512)
        {
            programRowConfig.destAddr = (uint32_t*)u32FlashAddr;
            programRowConfig.dataAddr = (uint32_t*)pData;
            eStatus = Cy_Flash_ProgramRow(NULL, &programRowConfig, CY_FLASH_DRV_MODE);
            if (eStatus != CY_FLASH_DRV_SUCCESS) 
            {
                u32ProgError |= 0x00001u;
                break;
            }
            
            pData += CODE_FLASH_ROW_DATA_SIZE_512;
            u32FlashAddr += CODE_FLASH_ROW_DATA_SIZE_512;
        }
        
        programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_256BIT;
        for (; u32Remaining >= CODE_FLASH_ROW_DATA_SIZE_32 ; u32Remaining-=CODE_FLASH_ROW_DATA_SIZE_32)
        {
            programRowConfig.destAddr = (uint32_t*)u32FlashAddr;
            programRowConfig.dataAddr = (uint32_t*)pData;
            eStatus = Cy_Flash_ProgramRow(NULL, &programRowConfig, CY_FLASH_DRV_MODE);
            if (eStatus != CY_FLASH_DRV_SUCCESS)
            {
                u32ProgError |= 0x00010u;
                break;
            }
            
            pData += CODE_FLASH_ROW_DATA_SIZE_32;
            u32FlashAddr += CODE_FLASH_ROW_DATA_SIZE_32;
        }
        
        programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_64BIT;
        for (; u32Remaining >= CODE_FLASH_ROW_DATA_SIZE_8 ; u32Remaining-=CODE_FLASH_ROW_DATA_SIZE_8)
        {
            programRowConfig.destAddr = (uint32_t*)u32FlashAddr;
            programRowConfig.dataAddr = (uint32_t*)pData;
            eStatus = Cy_Flash_ProgramRow(NULL, &programRowConfig, CY_FLASH_DRV_MODE);
            if (eStatus != CY_FLASH_DRV_SUCCESS)
            {
                u32ProgError |= 0x00100u;
                break;
            }
            
            pData += CODE_FLASH_ROW_DATA_SIZE_8;
            u32FlashAddr += CODE_FLASH_ROW_DATA_SIZE_8;
        }
        
        if (u32Remaining != 0)
        {
            programRowConfig.destAddr = (uint32_t*)u32FlashAddr;
            programRowConfig.dataAddr = (uint32_t*)u8LastRow;
            memcpy((void*)u8LastRow,(void*)pData,u32Remaining);
            eStatus = Cy_Flash_ProgramRow(NULL, &programRowConfig, CY_FLASH_DRV_MODE);
            if (eStatus != CY_FLASH_DRV_SUCCESS)
            {
                u32ProgError |= 0x00010u;
            }
        }
    
        if (0u != u32ProgError  )
            u8HalResult = DRIVER_FALSE;
        else
            u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_FMC_FlashRead (uint8_t *pu8Data,uint32_t u32Address,uint16_t  u16DataLength)
{
    uint8_t u8HalResult;
    if ((NULL != pu8Data)   &&\
        (u16DataLength > 0) &&\
        (Cy_Flash_MainBoundsCheck(u32Address) == CY_FLASH_IN_BOUNDS))
    {
        memcpy((void*)pu8Data,(void*)u32Address,u16DataLength);
        return DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_FMC_EEPROM_Safety( uint8_t u8Enable)
{
    uint8_t u8HalResult;
    
    if ((FUNC_ENABLE == u8Enable) || (FUNC_DISABLE == u8Enable))
    {
        if (FUNC_ENABLE == u8Enable)
            Cy_Flashc_WorkWriteDisable();
        else
            Cy_Flashc_WorkWriteEnable();
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_FMC_EEPROMEraseAll(uint8_t u8Verify)
{
    uint8_t u8HalResult;
    /*Erase wrok flash large sector*/
    for (uint32_t u32WSectorAddr = CY_WFLASH_LG_SBM_TOP ; 
         u32WSectorAddr < CY_WFLASH_LG_SBM_END ; 
         u32WSectorAddr += CY_WORK_LES_SIZE_IN_BYTE)
    {
        u8HalResult = HAL_FMC_EEPROMEraseSector(u32WSectorAddr,u8Verify);
        if (u8HalResult != DRIVER_TRUE)
            break;
    }
    
    if (DRIVER_TRUE == u8HalResult)
    {
        for (uint32_t u32WSectorAddr = CY_WFLASH_SM_SBM_TOP ; 
             u32WSectorAddr < CY_WFLASH_SM_SBM_SIZE ; 
             u32WSectorAddr += CY_WORK_SES_SIZE_IN_BYTE)
        {
            u8HalResult = HAL_FMC_EEPROMEraseSector(u32WSectorAddr,u8Verify);
            if (u8HalResult != DRIVER_TRUE)
                break;
        }
    }
    
    return u8HalResult;
}

uint8_t HAL_FMC_EEPROMEraseSector(uint32_t u32SectorAddr ,uint8_t u8Verify)
{
    uint8_t u8HalResult;
    cy_en_flashdrv_status_t eStatus;
    cy_stc_flash_erasesector_config_t eraseSectorConfig = {0};
    
     if(CY_FLASH_IN_BOUNDS == Cy_Flash_WorkBoundsCheck(u32SectorAddr) &&
       ((FUNC_ENABLE == u8Verify) || (FUNC_DISABLE == u8Verify)))
     {
         /* Erase Working flash sector and verify*/
         eraseSectorConfig.blocking = CY_FLASH_ERASESECTOR_BLOCKING;
         eraseSectorConfig.intrMask = CY_FLASH_ERASESECTOR_NOT_SET_INTR_MASK;
         eraseSectorConfig.Addr = (uint32_t*)u32SectorAddr;
         eStatus = Cy_Flash_EraseSector(NULL, &eraseSectorConfig, CY_FLASH_DRIVER_BLOCKING);
         
         if (CY_FLASH_DRV_SUCCESS == eStatus)
         {
             u8HalResult = FMC_WorkFlash_Blank_Check(u32SectorAddr);
         }
         else
         {
             u8HalResult = DRIVER_FALSE;
         }
     }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
 
    return u8HalResult;
}
#if 1
uint8_t HAL_FMC_EEPROMWrite(uint8_t *pu8Data, uint32_t u32Address,uint16_t  u16DataLength)
{
    cy_en_flashdrv_status_t eStatus;
    uint8_t u8HalResult;
    uint32_t u32ProgError = 0u;
    uint8_t  u8LastRow[WORK_FLASH_ROW_DATA_SIZE] = {0u};
    cy_stc_flash_programrow_config_t programRowConfig = {0};

    /*  Program work flash*/
    programRowConfig.blocking = CY_FLASH_PROGRAMROW_BLOCKING;
    programRowConfig.skipBC   = CY_FLASH_PROGRAMROW_SKIP_BLANK_CHECK;
    programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_32BIT;
    programRowConfig.dataLoc  = CY_FLASH_PROGRAMROW_DATA_LOCATION_SRAM;
    programRowConfig.intrMask = CY_FLASH_PROGRAMROW_NOT_SET_INTR_MASK;
    
    if ((NULL != pu8Data)   &&\
        (u16DataLength > 0) &&\
        ( FLASHC_FM_CTL_ECT->unWORK_FLASH_SAFETY.stcField.u1WORKFLASHWRITEENABLE == 1u) &&\
        (Cy_Flash_WorkBoundsCheck(u32Address) == CY_FLASH_IN_BOUNDS))
    {
        uint8_t* pData = pu8Data; 
        uint32_t u32Remaining = (uint32_t) u16DataLength; 
        uint32_t u32FlashAddr =  u32Address;  
        for (; u32Remaining >= WORK_FLASH_ROW_DATA_SIZE ; u32Remaining-=WORK_FLASH_ROW_DATA_SIZE)
        {
            programRowConfig.destAddr = (uint32_t*)u32FlashAddr;
            programRowConfig.dataAddr = (uint32_t*)pData;
            eStatus = Cy_Flash_ProgramRow(NULL, &programRowConfig, CY_FLASH_DRIVER_BLOCKING);
            if(eStatus != CY_FLASH_DRV_SUCCESS)
            {
                u32ProgError |= 0x00010u;
                break;
            }
            pData += WORK_FLASH_ROW_DATA_SIZE;
            u32FlashAddr += WORK_FLASH_ROW_DATA_SIZE;
        }
        
        if (u32Remaining != 0)
        { 
            programRowConfig.destAddr = (uint32_t*)u32FlashAddr;
            programRowConfig.dataAddr = (uint32_t*)u8LastRow;
            memcpy((void*)u8LastRow,(void*)pData,u32Remaining);
            eStatus = Cy_Flash_ProgramRow(NULL, &programRowConfig, CY_FLASH_DRIVER_BLOCKING);
            if(eStatus != CY_FLASH_DRV_SUCCESS)
            {
                u32ProgError |= 0x00100u;
            }
        }
        
        if (0u != u32ProgError  )
            u8HalResult = DRIVER_FALSE;
        else
            u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}
#endif
uint8_t HAL_FMC_EEPROMRead (uint8_t *pu8Data,uint32_t u32Address,uint16_t  u16DataLength)
{
    uint8_t u8HalResult;
    if ((NULL != pu8Data)   &&\
        (u16DataLength > 0) &&\
        (Cy_Flash_WorkBoundsCheck(u32Address) == CY_FLASH_IN_BOUNDS) &&\
        (FMC_WorkFlash_Blank_Check(u32Address) == DRIVER_FALSE))
    {
        memcpy((void*)pu8Data ,(void*)u32Address,u16DataLength);
        return DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}
/*** End of file ***/
  
