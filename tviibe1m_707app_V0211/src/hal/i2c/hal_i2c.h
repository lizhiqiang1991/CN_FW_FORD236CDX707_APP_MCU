/***************************************************************************
*\file hal_i2c.h 
****************************************************************************/

#ifndef HAL_I2C_H
#define HAL_I2C_H

/** Master/Slave mode I2C Init/Deinit/Active*/
uint8_t HAL_I2C_Init   (uint8_t u8I2cGroup, i2c_config_t* tsConfig);
uint8_t HAL_I2C_DeInit (uint8_t u8I2cGroup);
uint8_t HAL_I2C_Active (uint8_t u8I2cGroup, uint8_t u8Enable);

/** I2C MASTER MODE APIs*/
/*Low level API (Master Mode)*/
uint8_t HAL_I2C_Master_Start_Send     (uint8_t u8I2cGroup,uint8_t u8Address,uint8_t u8ReadWrite);
uint8_t HAL_I2C_Master_ReStart_Send   (uint8_t u8I2cGroup,uint8_t u8Address,uint8_t u8ReadWrite);
uint8_t HAL_I2C_Master_Stop_Send      (uint8_t u8I2cGroup);
uint8_t HAL_I2C_Master_DataByte_Write (uint8_t u8I2cGroup,uint8_t  u8TxData,uint32_t* pStatus);
uint8_t HAL_I2C_Master_DataByte_Read  (uint8_t u8I2cGroup,uint8_t* pRxData,uint32_t u32AckNack,uint32_t* pStatus);

/** High level API (Master Mode Continuous write/read I2C device )*/
uint8_t HAL_I2C_Master_Transmit     (uint8_t u8I2cGroup,i2c_data_t* pTx, uint32_t* pStatus);
uint8_t HAL_I2C_Master_Receive      (uint8_t u8I2cGroup,i2c_data_t* pRx, uint32_t* pStatus);

uint8_t HAL_I2C_Master_Reg_Transmit (uint8_t u8I2cGroup,i2c_data_t* pTx, uint32_t* pStatus);
uint8_t HAL_I2C_Master_Reg_Receive  (uint8_t u8I2cGroup,i2c_data_t* pRx, uint32_t* pStatus);

uint8_t HAL_I2C_Master_Mem_Transmit (uint8_t u8I2cGroup,i2c_data_t* pTx, uint32_t* pStatus);
uint8_t HAL_I2C_Master_Mem_Receive  (uint8_t u8I2cGroup,i2c_data_t* pRx, uint32_t* pStatus);

/** I2C SLAVE MODE APIs*/
uint8_t HAL_I2C_Slave_ReadBuf_Config         (uint8_t u8I2cGroup,uint8_t *pRdBuf, uint32_t u32Size);
uint8_t HAL_I2C_Slave_WriteBuf_Config        (uint8_t u8I2cGroup,uint8_t *pWrBuf, uint32_t u32Size);
uint8_t HAL_I2C_Slave_WriteTransferCount_Get (uint8_t u8I2cGroup,uint32_t* pRecvSize);
uint8_t HAL_I2C_Slave_Status_Get             (uint8_t u8I2cGroup,uint32_t* pStatus);
uint8_t HAL_I2C_Callback_Register            (uint8_t u8I2cGroup,I2C_ISR pI2cCallback);
uint8_t HAL_I2C_Slave_Interrupt_Set          (uint8_t u8I2cGroup,uint8_t u8Enable, uint8_t u8Priority);

#endif

  