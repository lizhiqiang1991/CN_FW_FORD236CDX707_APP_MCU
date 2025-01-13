/*
* FlashApp.c
*
*  Created on: 2021-06-10
*      Author: JoshuaLuo
*/


//---------------------------- Include File ---------------------------------//


#include "cy_project.h"
#include "cy_device_headers.h"

/* include public information */
#include "gPinDef.h"

/* Include App */
#include "FlashApp.h"



//---------------------------- Declare Global Variable ------------------------//


//uint8_t gMcuVerReturn[11] = "1p0_2_10_00";																											// ECU Version is 1p0_1_03_00 -------

__root const uint8_t ucMcuVer[11] __attribute__((section(".sw_version"))) = {0x31, 0x70, 0x30, 0x5F, 0x31, 0x5F, 0x30, 0x33, 0x5F, 0x30, 0x30};	    // ECU Version is 1p0_1_03_00 -------
//__root const uint8_t ucHwVer[9] __attribute__((section(".hw_version"))) = {0x56, 0x30, 0x30, 0x2E, 0x30, 0x30, 0x2E, 0x30, 0x30};   				// HW Version is V00.00.00
__root const uint8_t ucMcuChecksum[] __attribute__((section(".mcu_checksum"))) = {0x2e, 0x6d, 0xaa, 0xe7,
                                                                                   0x00, 0x00, 0x00, 0x00,
																				   0x00, 0x00, 0x00, 0x00,
																				   0x00, 0x00, 0xf6, 0x70};
__root const uint8_t ucCustomID[7] __attribute__((section(".customer_id"))) = {0x41, 0x55, 0x4F, 0x46, 0x6F, 0xda, 0xf4};           // AUOFord

//CUSTOMER_ID_SECTION uint8_t ucCustomID[7] = {0x41, 0x55, 0x4F, 0x46, 0x6F, 0x72, 0x64};				// AUOFord
//MCU_CHECKSUM_SECTION uint8_t ucMcuChecksum[4] = {0x00, 0x01, 0x95, 0x24};
//SW_VERSION_SECTION uint8_t ucMcuVer[9] = {0x56, 0x30, 0x31, 0x2E, 0x30, 0x30, 0x2E, 0x30, 0x31};	// MCU Version is V01.00.01
//---------------------------- Start Program ---------------------------------//

static uint8_t u8flashWriteStatus = NUM_ZERO;

void Flash_Initialize(void)
{
    // Wait 3 [s] to avoid breaking debugger connection.
    //Cy_SysTick_DelayInUs(3000000);
    
    // Initialization 
    Cy_FlashInit(false /*non-blocking*/);           //try to use Non blocking mode :true by Adolf
    
}


// In this routine, whole one sector erased/written/read/verified
uint8_t Flash_Sector_Erase(uint32_t SectorAddr)
{   
    uint8_t checkStatus = false;
    /** Erasing **/
    // Erase
    Cy_FlashSectorErase(SectorAddr, CY_FLASH_DRIVER_BLOCKING);
    
    // Verify
    checkStatus = Cy_WorkFlashBlankCheck(SectorAddr, CY_FLASH_DRIVER_BLOCKING);
    
    return checkStatus;
}


uint8_t Flash_Work_Sector_Write(uint32_t SectorAddr, uint32_t SectorSizeInByte, uint8_t * p_ProgramBuffData, uint32_t DataSizeInByte)
{
    uint32_t* p_TestFlsTop = (uint32_t*)SectorAddr;
    
    uint32_t SectorSizeInWord = SectorSizeInByte / 4UL;
    uint32_t ProgDataSize_In_Word = DataSizeInByte / 4UL;
    
    uint32_t* pProgramData = (uint32_t *)p_ProgramBuffData;
    
    uint32_t u32WwordId = 0UL; 
    
    uint32_t u32Addr =0UL;
    uint32_t u32AddrOffset =0UL;
    
    uint8_t checkStatus = false;    
    uint32_t u32DataPos =0UL;
    
    checkStatus = Flash_Sector_Erase(SectorAddr);
    
    if(checkStatus != true)
    {
        ;
    }  
    /** Programming 32bit at once**/
    for(u32Addr = SectorAddr, u32AddrOffset = 0UL; u32Addr < (SectorAddr + SectorSizeInByte); u32Addr+=4UL, u32AddrOffset+=4UL)
    {
        u32DataPos = u32AddrOffset % DataSizeInByte;
        
        // Flash             
        Cy_FlashWriteWork(u32Addr, (uint32_t*)&p_ProgramBuffData[u32DataPos], CY_FLASH_DRIVER_BLOCKING);
//        Cy_SysLib_DelayUs(100);                 //Append delay by Adolf
    }
    
    // Verify 
    for(u32WwordId = 0UL; u32WwordId < SectorSizeInWord; u32WwordId++)
    {
        u32DataPos = u32WwordId % ProgDataSize_In_Word;
        if(p_TestFlsTop[u32WwordId] == pProgramData[u32DataPos])
        {
            u8flashWriteStatus = true;
        }
        else
        {
            u8flashWriteStatus = false;
        }
    }
    
    
    return u8flashWriteStatus;
}

void Flash_Work_Sector_Read(uint32_t SectorAddr, uint32_t SectorSizeInByte, uint8_t * p_ProgramBuffData, uint32_t DataSizeInByte)
{
    uint32_t* p_TestFlsTop = (uint32_t*)SectorAddr;
    
    uint32_t SectorSizeInWord = SectorSizeInByte / 4UL;
    uint32_t ProgDataSize_In_Word = DataSizeInByte / 4UL;
    
    uint32_t* pProgramData = (uint32_t*)p_ProgramBuffData;
    uint32_t u32WwordId = 0UL; 
    uint32_t u32DataPos = 0UL;    
    uint8_t checkStatus = false; 
#if 1 //try to test for FLash check by Adolf
    checkStatus = Cy_WorkFlashBlankCheck(SectorAddr, CY_FLASH_DRIVER_BLOCKING);   //try to NON BLOCKING mode by Adolf
    if(checkStatus == true)
    {
        // flash is blank, do nothing.
        ;
    }
    else
    {
#endif
        // Read 
        for(u32WwordId = 0UL; u32WwordId < SectorSizeInWord; u32WwordId++)
        {   
            u32DataPos = u32WwordId % ProgDataSize_In_Word;
            
            pProgramData[u32DataPos] = p_TestFlsTop[u32WwordId];       
            
        }
#if 1   //try to test for FLash check by Adolf
    }
#endif    
}
//-------------------------------------------------------------------------//



