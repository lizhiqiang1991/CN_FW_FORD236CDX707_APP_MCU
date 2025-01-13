/**
* @file I2CMasterApp.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-03-18
* 
* @copyright Copyright (c) 2021
* 
*/
#include "Mod_DetectI2C.h"
#include "ICDiagApp.h"
#include "I2CMasterApp.h"
#include "hal_i2c_def.h"
#include <stdio.h>

#define I2C_MASTER_GROUP I2C_GROPU5
static uint8_t I2cMasterBuffer[I2SM_RBUFFER] = {NUM_ZERO};
static uint8_t I2cMasterInitStatus = DISABLE;

void I2CM_Initialze(void)
{   
    uint8_t u8HalResult  = 0U;
    gtsI2C_Master_Fast_Mode.u8PinSCL = PIN_I2C_SCB5_SCL;
    gtsI2C_Master_Fast_Mode.u8PinSDA = PIN_I2C_SCB5_SDA;
    gtsI2C_Master_Fast_Mode.u8SlaveAddress = 0x50;
    u8HalResult  = HAL_I2C_Init   (I2C_MASTER_GROUP,&gtsI2C_Master_Fast_Mode);
    if(u8HalResult != DRIVER_TRUE)
    {
        ;
    }
    I2cMasterInitStatus = ENABLE;
    
    // printf("I2C Master Init = %d\n",u8HalResult);
    HAL_I2C_Active(I2C_MASTER_GROUP,FUNC_ENABLE);
    
}

void I2CM_DeInitialze(void)
{
    (void)HAL_I2C_DeInit (I2C_MASTER_GROUP);
    
    I2cMasterInitStatus = DISABLE;
}


uint8_t I2CM_Write(uint8_t Device_ID, uint8_t Reg_addr, uint8_t * p_WData, uint8_t u8Length)
{
    uint32_t u32Status = 0U;
    uint8_t u8ret = 0U;
    i2c_data_t i2c_pak;
    
    i2c_pak.u8SlaveAddress = Device_ID;
    i2c_pak.u8RegAddress  = Reg_addr;
    i2c_pak.pData = p_WData;
    i2c_pak.u8Length = u8Length;
    
    if(I2cMasterInitStatus == ENABLE)
    {
        u8ret = HAL_I2C_Master_Reg_Transmit(I2C_MASTER_GROUP,&i2c_pak,&u32Status);
        if(u8ret != DRIVER_TRUE)
        {
            HAL_I2C_Master_Stop_Send(I2C_MASTER_GROUP);
        }
    }
    
    return u8ret;
}

uint8_t I2CM_Read(uint8_t Device_ID, uint8_t Reg_addr, uint8_t * p_WData, uint8_t u8Length)
{
    uint32_t u32Status = 0U;
    uint8_t u8ret = 0U;
    i2c_data_t i2c_pak;
    
    i2c_pak.u8SlaveAddress = Device_ID;
    i2c_pak.u8RegAddress  = Reg_addr;
    i2c_pak.pData = p_WData;
    i2c_pak.u8Length = u8Length;    
    
    
    if(I2cMasterInitStatus == ENABLE)
    {
        u8ret = HAL_I2C_Master_Reg_Receive(I2C_MASTER_GROUP,&i2c_pak,&u32Status);
        if(u8ret != DRIVER_TRUE)
        {
            HAL_I2C_Master_Stop_Send(I2C_MASTER_GROUP);
        }
    }
    
    return u8ret;
}

void I2CM_TEST_API(void)
{
    /** HAL_I2C_Master_Reg_Transmit / HAL_I2C_Master_Reg_Receive*/
    uint8_t u8RegData[3]  = {0x03,0,0};
    uint8_t u8ReadRegData[3]  = {0};
    uint32_t u32Status = 0u;
    
    //   /*Write to device address 0x50 , register 0 , data 0x55u*/
    //   /*Start bit*/
    //   CY_ASSERT( HAL_I2C_Master_Start_Send(I2C_GROPU0,0x50,I2C_ACTION_WRITE) == DRIVER_TRUE);
    //   /*Set register 0*/
    //   CY_ASSERT( HAL_I2C_Master_DataByte_Write(I2C_GROPU0,0x00,&u32Status) == DRIVER_TRUE);
    //   /*Write to register 0 data 0xaa*/
    //   CY_ASSERT( HAL_I2C_Master_DataByte_Write(I2C_GROPU0,u8RegData,&u32Status) == DRIVER_TRUE);
    //   /*Write Stop bit*/
    //   CY_ASSERT( HAL_I2C_Master_Stop_Send(I2C_GROPU0) == DRIVER_TRUE);
    //   printf("Write Register 0 = 0x%x\n",u8RegData);
    
    i2c_data_t i2c_pak = 
    {
        .u8SlaveAddress = 0x71,
        .u8RegAddress  = 0x04u,
        .pData = u8RegData,
        .u8Length = 1u,
    };
    CY_ASSERT ( HAL_I2C_Master_Reg_Transmit(I2C_GROPU5,&i2c_pak,&u32Status) == DRIVER_TRUE);
    printf("HAL_I2C_Master_Reg_Write Register 0 = 0x%x\n",*u8RegData);
    
    /*Read it back device address 0x28 , register 0*/
    (void) memset(u8ReadRegData, NULL, I2SM_RBUFFER); /*clear data*/
    i2c_pak.u8SlaveAddress = 0x71;
    i2c_pak.u8RegAddress  = 0x04u;
    i2c_pak.pData = u8ReadRegData;
    i2c_pak.u8Length = 2;
    
    CY_ASSERT( HAL_I2C_Master_Reg_Receive(I2C_GROPU0,&i2c_pak,&u32Status) == DRIVER_TRUE);
    for(int i = 0 ; i < i2c_pak.u8Length ;i++)
        printf("HAL_I2C_Master_Reg_Read Register 0 = 0x%x\n\n",u8ReadRegData[i]); /**<-- u8RegData should be 0x55*/
    
    Cy_SysLib_Delay(1300);
    
}



/******************************************************************************
;       Function Name    :       uint8_t I2C_TconApp_Magic_Code_Write(uint8_t u8TconAddr)
;       Function Description       :       Set Magic Code
;       Parameters                             :       [u8TconAddr] - Cmd Address
;       Return Values                 :       I2C
******************************************************************************/
uint8_t I2C_TconApp_Magic_Code_Write(uint8_t u8TconAddr)
{
    uint8_t u8Status= false;
    uint8_t u8Wr_Data = TCON_MAGIC_CODE;
    
    if (DRIVER_TRUE == I2CM_Write(TCON_ADDRESS_ID, u8TconAddr, &u8Wr_Data, sizeof(u8Wr_Data)))
    {
        u8Status = true;
    }
    else
    {
        u8Status = false;
    }
    
    return u8Status;
}


/* I2C_TCON_HX8880 , use for > 2ms timer*/
uint8_t I2CM_TCON_Reg_Read(uint8_t Reg_Addr, uint8_t * p_u8Data)
{
    uint16_t u16CheckSum = NUM_ZERO;
    uint8_t u8Status= false;
    static I2C_TconStrategyState_E I2C_TconStrategyState_e = eI2C_TCON_Magic_Wr;
    uint8_t u8Idx = NUM_ZERO;
    uint8_t u8Length = TCON_MAGICCODE_LENGTH + TCON_CHECKSUM_LENGTH;
    uint8_t u8CMDLength = NUM_ZERO;
    if (I2cMasterBuffer != NULL)
    {
        (void)memset(I2cMasterBuffer,0,sizeof(I2cMasterBuffer));
    }
    
    switch(Reg_Addr)
    {
    case TCON_CHIPID_ADDR:
        u8CMDLength = TCON_CHIPID_LENGTH;
        u8Length += TCON_CHIPID_LENGTH;
        break;
        
    case TCON_GLOBAL_DUTY_ADDR_R:
        u8Length += TCON_GLOBALDUTY_LENGTH;
        u8CMDLength = TCON_GLOBALDUTY_LENGTH;
        break;        
        
    case TCON_FAIL_FLAG_ADDR:
        u8Length += TCON_FAIL_FLAG_LENGTH;
        u8CMDLength = TCON_FAIL_FLAG_LENGTH;
        break;       
        
    default:
        /* Nothing */
        break;
    }
    
    switch(I2C_TconStrategyState_e)
    {
    case eI2C_TCON_Magic_Wr:                
        if(MDI2C_TconApp_Magic_Code_Write(Reg_Addr) == true )
        {
            ;	
        }
        I2C_TconStrategyState_e = eI2C_Wait2ms_T1;
        break;
        
    case eI2C_Wait2ms_T1:
        I2C_TconStrategyState_e = eI2C_Wait2ms_T2;
        break;
        
    case eI2C_Wait2ms_T2:
        I2C_TconStrategyState_e = eI2C_Wait2ms_T3;	
        break;
        
    case eI2C_Wait2ms_T3:
        I2C_TconStrategyState_e = eI2C_TCON_Reg_Read;	
        break;
        
    case eI2C_TCON_Reg_Read:
        (void)I2CM_Read(TCON_ADDRESS_ID, Reg_Addr, &I2cMasterBuffer[0], u8Length);
        if(I2cMasterBuffer[0] == TCON_MAGIC_CODE)
        {
            for(u8Idx = NUM_ZERO; u8Idx < u8Length; u8Idx++)
            {
                u16CheckSum += (uint16_t)I2cMasterBuffer[u8Idx];
            }
            u16CheckSum = u16CheckSum & 0x00FFU;
            if(I2cMasterBuffer[u8Length - 1U] == (uint8_t)u16CheckSum)
            {
                for(u8Idx = 1U; u8Idx <= u8CMDLength; u8Idx++)
                {
                    p_u8Data[u8Idx-1U] = (uint16_t)I2cMasterBuffer[u8Idx];
                }
                u8Status = true;
            }
        }				
        
        I2C_TconStrategyState_e = eI2C_TCON_Magic_Wr;	
        break;	
        
    default:
        I2C_TconStrategyState_e = eI2C_TCON_Magic_Wr;
        break;
        
    }
    
    return u8Status;
}

/* I2C_TCON_HX8880 */
uint8_t I2CM_TCON_Reg_Write(uint8_t Reg_Addr, uint8_t * p_u8Data)
{
    uint16_t u16CheckSum = NUM_ZERO;
    uint8_t u8Status= false;
    uint8_t u8Idx = NUM_ZERO;
    uint8_t u8Length = NUM_ZERO ;
    
    if (I2cMasterBuffer != NULL)
    {
        (void)memset(I2cMasterBuffer,0,sizeof(I2cMasterBuffer));
    }
    
    switch(Reg_Addr)
    {        
    case TCON_GLOBAL_DUTY_ADDR_W:
        u8Length += TCON_GLOBALDUTY_LENGTH;
        break;      
        
    default:
        /* Nothing */
        break;
    }
    
    u16CheckSum = (uint16_t)Reg_Addr;
    for(u8Idx = NUM_ZERO; u8Idx < u8Length; u8Idx++)
    {
        u16CheckSum += (uint16_t)(p_u8Data[u8Idx]);
        I2cMasterBuffer[u8Idx] = p_u8Data[u8Idx];
    }
    I2cMasterBuffer[u8Length] = (uint8_t)(u16CheckSum & 0x00FFU);
    
    u8Length += TCON_CHECKSUM_LENGTH;
    if (DRIVER_TRUE == I2CM_Write(TCON_ADDRESS_ID, Reg_Addr, &I2cMasterBuffer[NUM_ZERO], u8Length))
    {
        u8Status = true;
    }
    else
    {
        u8Status = false;
    }         			
    
    return u8Status;
}

#if BACKDOOR_ICDIAG_OPEN
void I2CM_Diag(uint8_t u8DeviceAddr, uint8_t u8DataAddr, uint8_t *u8TxData, uint8_t u8TxLen, uint8_t *u8RxData, uint8_t u8RxLen)
{
    //uint32_t u32TxLen=(uint32_t)u8TxLen;
    uint32_t u32RxLen=(uint32_t)u8RxLen;
    uint8_t u8I2cTxResult;
    uint8_t u8I2cRxResult;
    uint8_t u8I2cResult;
    
    if(u32RxLen > 0) 
    {
        /* Modify to match read Tcon by Adolf 20220815*/
        if(u8DeviceAddr == TCON_ADDRESS_ID)
        {
            /* Modify to match read Tcon by Adolf 20220815*/
            *u8TxData = TCON_MAGIC_CODE;
            /* Fetch Data */
            u8I2cTxResult = I2CM_Write(u8DeviceAddr, u8DataAddr, u8TxData, u8TxLen);
            Cy_SysLib_Delay(5);
            u8I2cRxResult = I2CM_Read(u8DeviceAddr, u8DataAddr, u8RxData, u8RxLen);
//            memcpy(u8RxData , (u8RxData + 2) , u8RxLen);
        }
        else
        {
            /* Fetch Data */
            u8I2cTxResult = I2CM_Write(u8DeviceAddr, u8DataAddr, u8TxData, u8TxLen);
            Cy_SysLib_Delay(5);
            u8I2cRxResult = I2CM_Read(u8DeviceAddr, u8DataAddr, u8RxData, u8RxLen);
        }
        u8I2cResult = u8I2cTxResult && u8I2cRxResult;
    }
    else 
    {
        /* Write Data */
        u8I2cResult = I2CM_Write(u8DeviceAddr, u8DataAddr, u8TxData, u8TxLen);
        
    }
    
    if(u8I2cResult == DRIVER_TRUE)
    {
        ICDIAG_SetCmdResault(ICDIAG_RESULT_SUCCESS);
    }
    else
    {
        ICDIAG_SetCmdResault(ICDIAG_RESULT_FAIL);
    }
  
}
#endif