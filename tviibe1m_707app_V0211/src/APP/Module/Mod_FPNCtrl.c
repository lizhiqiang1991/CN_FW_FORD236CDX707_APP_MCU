/**
* @file Mod_FPNCtrl.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-02-24
* 
* @copyright Copyright (c) 2021
* 
*/
/* include module */
#include"Mod_FPNCtrl.h"

/* Include App */
#include "FlashApp.h"

/* Declare Global value */
static uint8_t DelAssmFPNReadCount = NUM_ZERO;
static FPN_T FPN_DelAssm_t;

static uint8_t CoreAssmFPNReadCount = NUM_ZERO;
static FPN_T FPN_CoreAssm_t;

static uint8_t MainCalibFPNReadCount = NUM_ZERO;
static FPN_T FPN_MainCalib_t;

static uint8_t DispIDFPNFPNReadCount = NUM_ZERO;
static FPN_DISPLAY_ID_T FPN_DispID_t;

static uint8_t SoftwareFPNFPNReadCount = NUM_ZERO;
static FPN_T FPN_Software_t;

/* leo 20220616*/
static uint8_t SerialNumFPNReadCount = NUM_ZERO;
static FPN_T FPN_SerialNum_t;

static uint8_t u8FPNFlashBuff[128];



void MFPNC_FPNDelAssmToFlash_Write(uint8_t *pFPN,uint8_t u8Size)
{    
    (void)memset(u8FPNFlashBuff, 0xFFU, sizeof(u8FPNFlashBuff));
    memcpy(u8FPNFlashBuff,pFPN,u8Size);
    (void)Flash_Work_Sector_Write(DeliveryAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,u8FPNFlashBuff, (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
}

void MFPNC_FPNDelAssmFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size)
{
    (void)memset(u8FPNFlashBuff, NUM_ZERO, sizeof(u8FPNFlashBuff));
    Flash_Work_Sector_Read(DeliveryAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FPNFlashBuff[0], (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
    memcpy(pFPNBuff,&u8FPNFlashBuff[0],u8Size);
}

void MFPNC_FPNCoreAssmToFlash_Write(uint8_t *pFPN,uint8_t u8Size)
{    
    (void)memset(u8FPNFlashBuff, 0xFFU, sizeof(u8FPNFlashBuff));
    memcpy(u8FPNFlashBuff,pFPN,u8Size);
    (void)Flash_Work_Sector_Write(CoreAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,u8FPNFlashBuff, (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
}

void MFPNC_FPNCoreAssmFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size)
{
    (void)memset(u8FPNFlashBuff, NUM_ZERO, sizeof(u8FPNFlashBuff));
    Flash_Work_Sector_Read(CoreAssemblyAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FPNFlashBuff[0], (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
    memcpy(pFPNBuff,&u8FPNFlashBuff[0],u8Size);
}

void MFPNC_FPNMainCalibToFlash_Write(uint8_t *pFPN,uint8_t u8Size)
{    
    (void)memset(u8FPNFlashBuff, 0xFFU, sizeof(u8FPNFlashBuff));
    memcpy(u8FPNFlashBuff,pFPN,u8Size);
    (void)Flash_Work_Sector_Write(MainCalibrationFordPartNumberAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,u8FPNFlashBuff, (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
}

void MFPNC_FPNMainCalibFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size)
{
    (void)memset(u8FPNFlashBuff, NUM_ZERO, sizeof(u8FPNFlashBuff));
    Flash_Work_Sector_Read(MainCalibrationFordPartNumberAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FPNFlashBuff[0], (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
    memcpy(pFPNBuff,&u8FPNFlashBuff[0],u8Size);
}

void MFPNC_FPNDispIDToFlash_Write(uint8_t *pFPN,uint8_t u8Size)
{    
    (void)memset(u8FPNFlashBuff, 0xFFU, sizeof(u8FPNFlashBuff));
    memcpy(u8FPNFlashBuff,pFPN,u8Size);
    (void)Flash_Work_Sector_Write(DisplayIDAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,u8FPNFlashBuff, (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
}

void MFPNC_FPNDispIDFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size)
{
    (void)memset(u8FPNFlashBuff, NUM_ZERO, sizeof(u8FPNFlashBuff));
    Flash_Work_Sector_Read(DisplayIDAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FPNFlashBuff[0], (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
    memcpy(pFPNBuff,&u8FPNFlashBuff[0],u8Size);
}

void MFPNC_FPNSoftwareToFlash_Write(uint8_t *pFPN,uint8_t u8Size)
{    
    (void)memset(u8FPNFlashBuff, 0xFFU, sizeof(u8FPNFlashBuff));
    memcpy(u8FPNFlashBuff,pFPN,u8Size);
    (void)Flash_Work_Sector_Write(SoftwareFordPartNumberAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,u8FPNFlashBuff, (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
}

void MFPNC_FPNSoftwareFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size)
{
    (void)memset(u8FPNFlashBuff, NUM_ZERO, sizeof(u8FPNFlashBuff));
    Flash_Work_Sector_Read(SoftwareFordPartNumberAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FPNFlashBuff[0], (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
    memcpy(pFPNBuff,&u8FPNFlashBuff[0],u8Size);
}

/* leo 20220621 */
void MFPNC_FPNSerialToFlash_Write(uint8_t *pFPN,uint8_t u8Size)
{
    (void)memset(u8FPNFlashBuff, 0xFFU, sizeof(u8FPNFlashBuff));
    memcpy(u8FPNFlashBuff,pFPN,u8Size);
    (void)Flash_Work_Sector_Write(SerialNumAddr, SIZE_WORK_FLASH_SSECTOR_BYTE,u8FPNFlashBuff, (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
}

void MFPNC_FPNSerialFromFlash_Read(uint8_t *pFPNBuff,uint8_t u8Size)
{
    (void)memset(u8FPNFlashBuff, NUM_ZERO, sizeof(u8FPNFlashBuff));
    Flash_Work_Sector_Read(SerialNumAddr, SIZE_WORK_FLASH_SSECTOR_BYTE, &u8FPNFlashBuff[0], (sizeof(u8FPNFlashBuff)/sizeof(uint8_t)));
    memcpy(pFPNBuff,&u8FPNFlashBuff[0],u8Size);
}

uint8_t MFPNC_FPNCheckSum_Calculate (uint8_t *pFPNBuff)
{
    uint8_t AddIndex = NUM_ZERO;
    uint8_t u32SumTemp = NUM_ZERO;
    uint8_t FPNbufftemp[FPN_SIZE];
    
    memcpy(FPNbufftemp,pFPNBuff,FPN_SIZE);
    
    /* get sum */
    for (AddIndex = NUM_ZERO;AddIndex < FPN_CHECKSUM_INDEX;AddIndex++)
    {
        u32SumTemp += FPNbufftemp[AddIndex];
    }
    
    return u32SumTemp;
    
}

uint8_t MFPNC_FPNCheckSum_DispID_Calculate (uint8_t *pFPNBuff)
{
    uint8_t AddIndex = NUM_ZERO;
    uint8_t u32SumTemp = NUM_ZERO;
    uint8_t FPNbufftemp[FPN_DISP_ID_SIZE];
    
    memcpy(FPNbufftemp,pFPNBuff,FPN_DISP_ID_SIZE);
    
    /* get sum */
    for (AddIndex = NUM_ZERO;AddIndex < FPN_CHECKSUM_DISP_ID_INDEX;AddIndex++)
    {
        u32SumTemp += FPNbufftemp[AddIndex];
    }
    
    return u32SumTemp;
    
}

uint8_t MFPNC_FPNCheckSum_Compare (uint8_t u8TCS,uint8_t u8SCS)
{
    uint8_t CheckReault = NUM_ZERO;
    
    if(u8TCS == u8SCS)
    {
        CheckReault = ENABLE;
    }
    else
    {
        CheckReault = DISABLE;
    }
    
    return CheckReault;
}

uint8_t MFPNC_FPNDelAssmWriteToFlash_Process(void)
{
    uint8_t *pDelivery_assembly = NULL;
    uint8_t  ProcessStatus = 0U;
    uint8_t u8Temp = NUM_ZERO;
    uint8_t u8CompResulat = NUM_ZERO;
    
    /* Write FPN to Flash */
    if(FPN_DelAssm_t.FPN_Flag_t.FPNCtrlFlag == ENABLE)
    {
        if(FPN_DelAssm_t.FPN_Flag_t.FPNDataFlag == ENABLE)
        {
            /* Caculate sum */
            u8Temp = MFPNC_FPNCheckSum_Calculate(FPN_DelAssm_t.pFPNDataBuff);
            
            /* Compare with checksum */
            u8CompResulat = MFPNC_FPNCheckSum_Compare(u8Temp,FPN_DelAssm_t.pFPNDataBuff[FPN_CHECKSUM_INDEX]) ;
            
            if(u8CompResulat == ENABLE)
            {
                pDelivery_assembly = FPN_DelAssm_t.pFPNDataBuff;
                MFPNC_FPNDelAssmToFlash_Write(pDelivery_assembly,FPN_DelAssm_t.FPNDataLength);    
                ProcessStatus = FPN_WRITE_SUCCESS;
            }
            else
            {
                ProcessStatus = FPN_CHECKSUM_ERR;
            }
        }
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        ProcessStatus = FPN_WRITE_FAIL;
    }
    
    return ProcessStatus;
}

uint8_t MFPNC_DelAssm_WRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = DISABLE;
    uint8_t u8ProcessState = DISABLE;
    
    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;
        
        if(u8WRTST == ENABLE)
        {
            
            u8ProcessState = DISABLE;
        }
        else
        {
            u8ProcessState = ENABLE;
            
        }
    }
    else
    {
        ;
    }
    
    return u8ProcessState;
}

uint8_t MFPNC_FPNDelAssmControl_Process(FPN_T *fpn_t)
{
    uint8_t  ProcessStatus = 0U;
    
    FPN_DelAssm_t = *fpn_t;
    
    if(MFPNC_DelAssm_WRTST_Check(FPN_DelAssm_t.FPN_Status_t.FPN_WRT_ST)== ENABLE)
    {
        FPN_DelAssm_t.FPN_Flag_t.FPNCtrlFlag = ENABLE;
    }
    else
    {
        FPN_DelAssm_t.FPN_Flag_t.FPNCtrlFlag = DISABLE;
    }
    
    /* Read FPN back from Flash */
    if(DelAssmFPNReadCount == NUM_ZERO)
    {
        /* Read back */
        MFPNC_FPNDelAssmFromFlash_Read(FPN_DelAssm_t.ReadFPNBuff,FPN_SIZE);
        /* copy to buffer */
        memcpy(fpn_t->ReadFPNBuff,FPN_DelAssm_t.ReadFPNBuff,FPN_SIZE);
        DelAssmFPNReadCount++;
        ProcessStatus = FPN_READ_SUCCESS;
    }
    else
    {    
        ProcessStatus = MFPNC_FPNDelAssmWriteToFlash_Process();
    }
    
    return ProcessStatus;   
}




uint8_t MFPNC_FPNDispIDWriteToFlash_Process(void)
{
    uint8_t *pDisplay_ID = NULL;
    uint8_t  ProcessStatus = 0U;
    uint8_t u8Temp = NUM_ZERO;
    uint8_t u8CompResulat = NUM_ZERO;
    
    /* Write FPN to Flash */
    if(FPN_DispID_t.FPN_Flag_t.FPNCtrlFlag == ENABLE)
    {
        // if(FPN_DispID_t.FPN_Status_t.FPN_WRT_ST == ENABLE)
        // {
        if(FPN_DispID_t.FPN_Flag_t.FPNDataFlag == ENABLE)
        {
            /* Caculate sum */
            u8Temp = MFPNC_FPNCheckSum_DispID_Calculate(FPN_DispID_t.pFPNDataBuff);
            
            /* Compare with checksum */
            u8CompResulat = MFPNC_FPNCheckSum_Compare(u8Temp,FPN_DispID_t.pFPNDataBuff[FPN_CHECKSUM_DISP_ID_INDEX]) ;
            
            if(u8CompResulat == ENABLE)
            {
                pDisplay_ID = FPN_DispID_t.pFPNDataBuff;
                MFPNC_FPNDispIDToFlash_Write(pDisplay_ID,FPN_DispID_t.FPNDataLength);    
                ProcessStatus = FPN_WRITE_SUCCESS;
            }
            else
            {
                ProcessStatus = FPN_CHECKSUM_ERR;
            }
        }
        // }
        
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        ProcessStatus = FPN_WRITE_FAIL;
    }
    
    return ProcessStatus;
}

uint8_t MFPNC_DispID_WRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = DISABLE;
    uint8_t u8ProcessState = DISABLE;
    
    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;
        
        if(u8WRTST == ENABLE)
        {
            
            u8ProcessState = DISABLE;
        }
        else
        {
            u8ProcessState = ENABLE;
            
        }
    }
    else
    {
        ;
    }
    
    return u8ProcessState;
}

uint8_t MFPNC_FPNDispIDControl_Process(FPN_DISPLAY_ID_T *fpn_t)
{
    uint8_t  ProcessStatus = 0U;
    
    FPN_DispID_t = *fpn_t;
    
    if(MFPNC_DispID_WRTST_Check(FPN_DispID_t.FPN_Status_t.FPN_WRT_ST)== ENABLE)
    {
        FPN_DispID_t.FPN_Flag_t.FPNCtrlFlag = ENABLE;
    }
    else
    {
        FPN_DispID_t.FPN_Flag_t.FPNCtrlFlag = DISABLE;
    }
    
    /* Read FPN back from Flash */
    if(DispIDFPNFPNReadCount == NUM_ZERO)
    {
        /* Read back */
        MFPNC_FPNDispIDFromFlash_Read(FPN_DispID_t.ReadFPNBuff,FPN_DISP_ID_SIZE);
        /* copy to buffer */
        memcpy(fpn_t->ReadFPNBuff,FPN_DispID_t.ReadFPNBuff,FPN_DISP_ID_SIZE);
        DispIDFPNFPNReadCount++;
        ProcessStatus = FPN_READ_SUCCESS;
    }
    else
    {    
        ProcessStatus = MFPNC_FPNDispIDWriteToFlash_Process();
    }
    
    return ProcessStatus;   
}


uint8_t MFPNC_FPNCoreAssmWriteToFlash_Process(void)
{
    uint8_t *pCore_assembly = NULL;
    uint8_t  ProcessStatus = 0U;
    uint8_t u8Temp = NUM_ZERO;
    uint8_t u8CompResulat = NUM_ZERO;
    
    /* Write FPN to Flash */
    if(FPN_CoreAssm_t.FPN_Flag_t.FPNCtrlFlag == ENABLE)
    {
        // if(FPN_CoreAssm_t.FPN_Status_t.FPN_WRT_ST == ENABLE)
        // {
        if(FPN_CoreAssm_t.FPN_Flag_t.FPNDataFlag == ENABLE)
        {
            /* Caculate sum */
            u8Temp = MFPNC_FPNCheckSum_Calculate(FPN_CoreAssm_t.pFPNDataBuff);
            
            /* Compare with checksum */
            u8CompResulat = MFPNC_FPNCheckSum_Compare(u8Temp,FPN_CoreAssm_t.pFPNDataBuff[FPN_CHECKSUM_INDEX]) ;
            
            if(u8CompResulat == ENABLE)
            {
                pCore_assembly = FPN_CoreAssm_t.pFPNDataBuff;
                MFPNC_FPNCoreAssmToFlash_Write(pCore_assembly,FPN_CoreAssm_t.FPNDataLength);    
                ProcessStatus = FPN_WRITE_SUCCESS;
            }
            else
            {
                ProcessStatus = FPN_CHECKSUM_ERR;
            }
        }
        // }
        
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        ProcessStatus = FPN_WRITE_FAIL;
    }
    
    return ProcessStatus;
}
uint8_t MFPNC_CoreAssm_WRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = DISABLE;
    uint8_t u8ProcessState = DISABLE;
    
    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;
        
        if(u8WRTST == ENABLE)
        {
            
            u8ProcessState = DISABLE;
        }
        else
        {
            u8ProcessState = ENABLE;
            
        }
    }
    else
    {
        ;
    }
    
    return u8ProcessState;
}
uint8_t MFPNC_FPNCoreAssmControl_Process(FPN_T *fpn_t)
{
    uint8_t  ProcessStatus = 0U;
    
    FPN_CoreAssm_t = *fpn_t;
    
    if(MFPNC_CoreAssm_WRTST_Check(FPN_CoreAssm_t.FPN_Status_t.FPN_WRT_ST)== ENABLE)
    {
        FPN_CoreAssm_t.FPN_Flag_t.FPNCtrlFlag = ENABLE;
    }
    else
    {
        FPN_CoreAssm_t.FPN_Flag_t.FPNCtrlFlag = DISABLE;
    }
    
    
    /* Read FPN back from Flash */
    if(CoreAssmFPNReadCount == NUM_ZERO)
    {
        /* Read back */
        MFPNC_FPNCoreAssmFromFlash_Read(FPN_CoreAssm_t.ReadFPNBuff,FPN_SIZE);
        /* copy to buffer */
        memcpy(fpn_t->ReadFPNBuff,FPN_CoreAssm_t.ReadFPNBuff,FPN_SIZE);
        CoreAssmFPNReadCount++;
        ProcessStatus = FPN_READ_SUCCESS;
    }
    else
    {    
        ProcessStatus = MFPNC_FPNCoreAssmWriteToFlash_Process();
    }
    
    return ProcessStatus;   
}

uint8_t MFPNC_FPNMainCalibWriteToFlash_Process(void)
{
    uint8_t *pMain_Calibration = NULL;
    uint8_t  ProcessStatus = 0U;
    uint8_t u8Temp = NUM_ZERO;
    uint8_t u8CompResulat = NUM_ZERO;
    
    /* Write FPN to Flash */
    if(FPN_MainCalib_t.FPN_Flag_t.FPNCtrlFlag == ENABLE)
    {
        // if(FPN_MainCalib_t.FPN_Status_t.FPN_WRT_ST == ENABLE)
        // {
        if(FPN_MainCalib_t.FPN_Flag_t.FPNDataFlag == ENABLE)
        {
            /* Caculate sum */
            u8Temp = MFPNC_FPNCheckSum_Calculate(FPN_MainCalib_t.pFPNDataBuff);
            
            /* Compare with checksum */
            u8CompResulat = MFPNC_FPNCheckSum_Compare(u8Temp,FPN_MainCalib_t.pFPNDataBuff[FPN_CHECKSUM_INDEX]) ;
            
            if(u8CompResulat == ENABLE)
            {
                pMain_Calibration = FPN_MainCalib_t.pFPNDataBuff;
                MFPNC_FPNMainCalibToFlash_Write(pMain_Calibration,FPN_MainCalib_t.FPNDataLength);    
                ProcessStatus = FPN_WRITE_SUCCESS;
            }
            else
            {
                ProcessStatus = FPN_CHECKSUM_ERR;
            }
        }
        // }
        
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        ProcessStatus = FPN_WRITE_FAIL;
    }
    
    return ProcessStatus;
}
uint8_t MFPNC_MainCalib_WRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = DISABLE;
    uint8_t u8ProcessState = DISABLE;
    
    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;
        
        if(u8WRTST == ENABLE)
        {
            
            u8ProcessState = DISABLE;
        }
        else
        {
            u8ProcessState = ENABLE;
            
        }
    }
    else
    {
        ;
    }
    
    return u8ProcessState;
}
uint8_t MFPNC_FPNMainCalibControl_Process(FPN_T *fpn_t)
{
    uint8_t  ProcessStatus = 0U;
    
    FPN_MainCalib_t = *fpn_t;
    
    if(MFPNC_MainCalib_WRTST_Check(FPN_MainCalib_t.FPN_Status_t.FPN_WRT_ST)== ENABLE)
    {
        FPN_MainCalib_t.FPN_Flag_t.FPNCtrlFlag = ENABLE;
    }
    else
    {
        FPN_MainCalib_t.FPN_Flag_t.FPNCtrlFlag = DISABLE;
    }
    
    /* Read FPN back from Flash */
    if(MainCalibFPNReadCount == NUM_ZERO)
    {
        /* Read back */
        MFPNC_FPNMainCalibFromFlash_Read(FPN_MainCalib_t.ReadFPNBuff,FPN_SIZE);
        /* copy to buffer */
        memcpy(fpn_t->ReadFPNBuff,FPN_MainCalib_t.ReadFPNBuff,FPN_SIZE);
        MainCalibFPNReadCount++;
        ProcessStatus = FPN_READ_SUCCESS;
    }
    else
    {    
        ProcessStatus = MFPNC_FPNMainCalibWriteToFlash_Process();
    }
    
    return ProcessStatus;   
}



uint8_t MFPNC_FPNSoftwareWriteToFlash_Process(void)
{
    uint8_t *pSoftware_FPN = NULL;
    uint8_t  ProcessStatus = 0U;
    uint8_t u8Temp = NUM_ZERO;
    uint8_t u8CompResulat = NUM_ZERO;
    
    /* Write FPN to Flash */
    if(FPN_Software_t.FPN_Flag_t.FPNCtrlFlag == ENABLE)
    {
        // if(FPN_Software_t.FPN_Status_t.FPN_WRT_ST == ENABLE)
        // {
        if(FPN_Software_t.FPN_Flag_t.FPNDataFlag == ENABLE)
        {
            /* Caculate sum */
            u8Temp = MFPNC_FPNCheckSum_Calculate(FPN_Software_t.pFPNDataBuff);
            
            /* Compare with checksum */
            u8CompResulat = MFPNC_FPNCheckSum_Compare(u8Temp,FPN_Software_t.pFPNDataBuff[FPN_CHECKSUM_INDEX]) ;
            
            if(u8CompResulat == ENABLE)
            {
                pSoftware_FPN = FPN_Software_t.pFPNDataBuff;
                MFPNC_FPNSoftwareToFlash_Write(pSoftware_FPN,FPN_Software_t.FPNDataLength);    
                ProcessStatus = FPN_WRITE_SUCCESS;
            }
            else
            {
                ProcessStatus = FPN_CHECKSUM_ERR;
            }
        }
        // }
        
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        ProcessStatus = FPN_WRITE_FAIL;
    }
    
    return ProcessStatus;
}

uint8_t MFPNC_Software_WRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = DISABLE;
    uint8_t u8ProcessState = DISABLE;
    
    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;
        
        if(u8WRTST == ENABLE)
        {
            
            u8ProcessState = DISABLE;
        }
        else
        {
            u8ProcessState = ENABLE;
            
        }
    }
    else
    {
        ;
    }
    
    return u8ProcessState;
}

uint8_t MFPNC_FPNSoftwareControl_Process(FPN_T *fpn_t)
{
    uint8_t  ProcessStatus = 0U;
    
    FPN_Software_t = *fpn_t;
    
    if(MFPNC_Software_WRTST_Check(FPN_Software_t.FPN_Status_t.FPN_WRT_ST)== ENABLE)
    {
        FPN_Software_t.FPN_Flag_t.FPNCtrlFlag = ENABLE;
    }
    else
    {
        FPN_Software_t.FPN_Flag_t.FPNCtrlFlag = DISABLE;
    }
    
    /* Read FPN back from Flash */
    if(SoftwareFPNFPNReadCount == NUM_ZERO)
    {
        /* Read back */
        MFPNC_FPNSoftwareFromFlash_Read(FPN_Software_t.ReadFPNBuff,FPN_SIZE);
        /* copy to buffer */
        memcpy(fpn_t->ReadFPNBuff,FPN_Software_t.ReadFPNBuff,FPN_SIZE);
        SoftwareFPNFPNReadCount++;
        ProcessStatus = FPN_READ_SUCCESS;
    }
    else
    {    
        ProcessStatus = MFPNC_FPNSoftwareWriteToFlash_Process();
    }
    
    return ProcessStatus;   
}

/* leo 20220621 */
uint8_t MFPNC_FPNSerialWriteToFlash_Process(void)
{
    uint8_t *pSerial_number = NULL;
    uint8_t ProcessStatus = 0U;
    uint8_t u8Temp = NUM_ZERO;
    uint8_t u8CompResulat = NUM_ZERO;
    
    /* Write FPN to Flash */
    if(FPN_SerialNum_t.FPN_Flag_t.FPNCtrlFlag == ENABLE)
    {
        if(FPN_SerialNum_t.FPN_Flag_t.FPNDataFlag == ENABLE)
        {
            /* Caculate sum */
            u8Temp = MFPNC_FPNCheckSum_Calculate(FPN_SerialNum_t.pFPNDataBuff);
            
            /* Compare with checksum */
            u8CompResulat = MFPNC_FPNCheckSum_Compare(u8Temp,FPN_SerialNum_t.pFPNDataBuff[FPN_CHECKSUM_INDEX]) ;
            
            if(u8CompResulat == ENABLE)
            {
                pSerial_number = FPN_SerialNum_t.pFPNDataBuff;
                MFPNC_FPNSerialToFlash_Write(pSerial_number,FPN_SerialNum_t.FPNDataLength);    
                ProcessStatus = FPN_WRITE_SUCCESS;
            }
            else
            {
                ProcessStatus = FPN_CHECKSUM_ERR;
            }
        }
    }
    else
    {
        /* if FPN has been write before FPNStatusRegFlag,it will clear flag.*/
        ProcessStatus = FPN_WRITE_FAIL;
    }
    
    return ProcessStatus;
}

uint8_t MFPNC_Serial_WRTST_Check(uint8_t u8WRTST)
{
    static uint8_t u8WRTSTState = DISABLE;
    uint8_t u8ProcessState = DISABLE;
    
    if(u8WRTSTState != u8WRTST)
    {
        u8WRTSTState = u8WRTST;
        
        if(u8WRTST == ENABLE)
        {
            
            u8ProcessState = DISABLE;
        }
        else
        {
            u8ProcessState = ENABLE;
            
        }
    }
    else
    {
        ;
    }
    
    return u8ProcessState;
}

uint8_t MFPNC_FPNSerialControl_Process(FPN_T *fpn_t)
{
    uint8_t  ProcessStatus = 0U;
    
    FPN_SerialNum_t = *fpn_t;
    
    if(MFPNC_Serial_WRTST_Check(FPN_SerialNum_t.FPN_Status_t.FPN_WRT_ST)== ENABLE)
    {
        FPN_SerialNum_t.FPN_Flag_t.FPNCtrlFlag = ENABLE;
    }
    else
    {
        FPN_SerialNum_t.FPN_Flag_t.FPNCtrlFlag = DISABLE;
    }
    
    
    /* Read FPN back from Flash */
    if(SerialNumFPNReadCount == NUM_ZERO)
    {
        /* Read back */
        MFPNC_FPNSerialFromFlash_Read(FPN_SerialNum_t.ReadFPNBuff,FPN_SIZE);
        /* copy to buffer */
        memcpy(fpn_t->ReadFPNBuff,FPN_SerialNum_t.ReadFPNBuff,FPN_SIZE);
        SerialNumFPNReadCount++;
        ProcessStatus = FPN_READ_SUCCESS;
    }
    else
    {    
        ProcessStatus = MFPNC_FPNSerialWriteToFlash_Process();
    }
    
    return ProcessStatus;   
}