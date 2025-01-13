/**
* @file I2CSlaveApp.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-02-02
* 
* @copyright Copyright (c) 2021
* 
*/

#include "I2CSlaveApp.h"
#include "stdio.h"
#include "gPinDef.h"
#include "Mod_DataManagement.h"
#include "Ford_Config.h"
#include "UartApp.h"
/* Include Backdoor */
#include "ICDiagApp.h"

#define I2C_SLAVE_GROUP I2C_GROPU1


static uint8_t rxBuffer[I2CS_WBUFFER_SIZE] = {NUM_ZERO}; /* Declaration of the Slave LPI2C RX buffer for Master Write*/
static uint8_t txBuffer[I2CS_RBUFFER_SIZE] = {NUM_ZERO}; /* Declaration of the Slave LPI2C TX buffer for Master Read*/
static uint8_t subAddress = NUM_ZERO;
static uint8_t u8RollingCnt = NUM_ZERO;
static uint8_t u8Crc8 = NUM_ZERO;

static uint32_t writingByteSize;
static uint32_t txDataLength;

static void I2CS_Callback(uint32_t u32Events)
{
#ifdef FuSa
  
#if BACKDOOR_ICDIAG_OPEN
    uint8_t *u8I2CICDiagBuffer;
#endif
    switch (u32Events)
    {
    case I2C_SLAVE_READ_EVENT:
        /* For Bootloader */
        if(MMIM_Update_Flag_Get() == true)
        {
            MMIM_Update_Flag_Set(false);
            txDataLength = MMIM_DataLength_Get(subAddress);
            MMIM_MessageData_Get(subAddress, txBuffer);
            (void) HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_GROUP, txBuffer, (txDataLength + SUB_ADDRESS_SIZE));
        }
        else
        {   

            txDataLength = MMIM_DataLength_FunSafe_Get(subAddress, u8RollingCnt, writingByteSize);
#if BACKDOOR_ICDIAG_OPEN
            //except case by backdoor read command, we pass it.
            if(subAddress == ICDIAG_CMD_READ)
            {
                u8I2CICDiagBuffer = ICDIAG_GetRxBuffer();
                *txBuffer = subAddress;
                (void)memcpy(&txBuffer[FIRST_DATA_BYTE], u8I2CICDiagBuffer, ICDIAG_BUFFERSIZE);        
                (void) HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_GROUP, txBuffer, (ICDIAG_BUFFERSIZE + SUB_ADDRESS_SIZE));
            }
            else
            {
#endif     
                MMIM_MessageData_FunSafe_Get(subAddress, u8RollingCnt, u8Crc8, txBuffer, writingByteSize);
                if(txDataLength == 0UL)
                {
                    (void) HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_GROUP, txBuffer, (txDataLength + SUB_ADDRESS_SIZE + ROLLING_COUNTER_SIZE + CRC8_SIZE));
                }
                else
                {
                    (void) HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_GROUP, txBuffer, (txDataLength + SUB_ADDRESS_SIZE  + ROLLING_COUNTER_SIZE + CRC8_SIZE));
                }
#if BACKDOOR_ICDIAG_OPEN
            }
#endif 
        }
        break;
        
    case I2C_SLAVE_WRITE_EVENT:
        //printf("I2C_SLAVE_WRITE_EVENT\n");
        HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_GROUP,&rxBuffer[0], I2CS_WBUFFER_SIZE);
        break;
        
    case I2C_SLAVE_RD_IN_FIFO_EVENT:
        // printf("I2C_SLAVE_RD_IN_FIFO_EVENT\n");
        break;
        
    case I2C_SLAVE_RD_BUF_EMPTY_EVENT:
        //printf("I2C_SLAVE_RD_BUF_EMPTY_EVENT\n");
        break;
        
    case I2C_SLAVE_RD_CMPLT_EVENT:
        // printf("I2C_SLAVE_RD_CMPLT_EVENT\n");
        /* Clear Read Buffer (use same buffer) */
        
        (void) memset(txBuffer, NULL, I2CS_RBUFFER_SIZE);
        /* Set Tx buffer */
        HAL_I2C_Slave_ReadBuf_Config (I2C_SLAVE_GROUP,&txBuffer[0], I2CS_WBUFFER_SIZE);
        break;
        
    case I2C_SLAVE_WR_CMPLT_EVENT://mcu 接受完成
        if(rxBuffer[0] != 0xF4)       //20211224 Joel
        {
            subAddress = rxBuffer[0]; /*Get I2C subAddress*/
            u8RollingCnt  = rxBuffer[1]; /*Get Host rolling counter*/
            u8Crc8 = rxBuffer[2]; /*Get I2C Crc8 value*/
            
            HAL_I2C_Slave_WriteTransferCount_Get (I2C_SLAVE_GROUP , &writingByteSize);
            
            //printf("write count = %d\r\n",writingByteSize); 
            
            if (writingByteSize >3U)
            {
                if(rxBuffer[0] == 0xE4)
                {
                    MMIM_I2cMessages_Set(subAddress,rxBuffer,writingByteSize);      
                }
                else
                {
                    MMIM_I2cMessages_FunSafe_Set(subAddress,rxBuffer,writingByteSize);
                }
                //for (int i = 0;i < writingByteSize;i++)
                //printf("GSM = %d\r\n",rxBuffer[i]);                
            }
            (void) memset(rxBuffer, NULL, I2CS_WBUFFER_SIZE);
            /* clear Rx buffer */
            HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_GROUP,&rxBuffer[0], I2CS_WBUFFER_SIZE);
        }
        else /* 20220708 Taylor */
        {
            subAddress = rxBuffer[0]; /*Get I2C subAddress*/
            //u8RollingCnt  = rxBuffer[1]; /*Get Host rolling counter*/
            //u8Crc8 = rxBuffer[2]; /*Get I2C Crc8 value*/
            
            HAL_I2C_Slave_WriteTransferCount_Get (I2C_SLAVE_GROUP , &writingByteSize);
        }  
        break;
        
    case I2C_SLAVE_ERR_EVENT:
        
        //printf("SLAVE_ERR_EVENT");
        
        break;
    default:
        break;
    }
#else
    switch (u32Events)
    {
    case I2C_SLAVE_READ_EVENT:
        //printf("I2C_SLAVE_READ_EVENT\n");
        
        txDataLength = MMIM_DataLength_Get(subAddress);
        MMIM_MessageData_Get(subAddress, txBuffer);
        (void) HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_GROUP, txBuffer, (txDataLength + SUB_ADDRESS_SIZE));
        
        break;
        
    case I2C_SLAVE_WRITE_EVENT:
        //printf("I2C_SLAVE_WRITE_EVENT\n");
        HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_GROUP,&rxBuffer[0], I2CS_WBUFFER_SIZE);
        
        break;
        
    case I2C_SLAVE_RD_IN_FIFO_EVENT:
        // printf("I2C_SLAVE_RD_IN_FIFO_EVENT\n");
        break;
        
    case I2C_SLAVE_RD_BUF_EMPTY_EVENT:
        //printf("I2C_SLAVE_RD_BUF_EMPTY_EVENT\n");
        break;
        
    case I2C_SLAVE_RD_CMPLT_EVENT:
        // printf("I2C_SLAVE_RD_CMPLT_EVENT\n");
        /* Clear Read Buffer (use same buffer) */
        
        (void) memset(txBuffer, NULL, I2CS_WBUFFER_SIZE);
        /* Set Tx buffer */
        HAL_I2C_Slave_ReadBuf_Config (I2C_SLAVE_GROUP,&txBuffer[0], I2CS_WBUFFER_SIZE);
        break;
        
    case I2C_SLAVE_WR_CMPLT_EVENT:
        if(rxBuffer[0] != 0xF4)         //20211224 Joel
        {
            subAddress = rxBuffer[0];/*Get I2C subAddress*/
            
            HAL_I2C_Slave_WriteTransferCount_Get (I2C_SLAVE_GROUP , &writingByteSize);
            
            //printf("write count = %d\r\n",writingByteSize);
            
            if (writingByteSize != 1U)
            {
                
                MMIM_I2cMessages_Set(subAddress,rxBuffer,writingByteSize);
                
                //for (int i = 0;i < writingByteSize;i++)
                //printf("GSM = %d\r\n",rxBuffer[i]);
                
                
            }
            (void) memset(rxBuffer, NULL, I2CS_WBUFFER_SIZE);
            /* clear Rx buffer */
            HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_GROUP,&rxBuffer[0], I2CS_WBUFFER_SIZE);
        }
        break;
        
    case I2C_SLAVE_ERR_EVENT:
        
        //printf("SLAVE_ERR_EVENT");
        
        break;
    default:
        break;
    }
#endif
}

void I2CS_Initialze(void)
{
    gtsI2C_Slave_Fast_Mode.u8PinSCL = PIN_I2C_SCB1_SCL;
    gtsI2C_Slave_Fast_Mode.u8PinSDA = PIN_I2C_SCB1_SDA;
    gtsI2C_Slave_Fast_Mode.u8SlaveAddress = 0x71;
    
    HAL_I2C_Init   (I2C_SLAVE_GROUP,&gtsI2C_Slave_Fast_Mode);
    
    /* Master write : Slave receive data put in to g_i2c_rx_buf*/
    HAL_I2C_Slave_WriteBuf_Config (I2C_SLAVE_GROUP,&rxBuffer[0], I2CS_WBUFFER_SIZE);
    
    /* Master read : Slave output data from g_i2c_tx_buf*/
    HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_GROUP,&txBuffer[0], I2CS_WBUFFER_SIZE);
    
    HAL_I2C_Slave_Interrupt_Set(I2C_SLAVE_GROUP, FUNC_ENABLE,0x00U);
    
    HAL_I2C_Callback_Register (I2C_SLAVE_GROUP, I2CS_Callback);
    
    HAL_I2C_Active (I2C_SLAVE_GROUP,FUNC_ENABLE);
}

void I2CS_DeInitialze(void)
{
    (void)HAL_I2C_DeInit (I2C_SLAVE_GROUP);
}
