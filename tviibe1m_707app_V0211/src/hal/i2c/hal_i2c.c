/***************************************************************************
*\file hal_i2c.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "cyhal_hwmgr.h"
#include "hal_gpio_def.h"
#include "hal_gpio.h"
#include "hal_i2c_def.h"
#include "hal_i2c.h"

#define DATARATE_I2C_FAST_PLUS_MODE   (1000000U)   /**< 1000k bps*/
#define DATARATE_I2C_FAST_MODE         (400000U)   /**< 400k bps*/
#define DATARATE_I2C_STANDARD_MODE     (100000U)   /**< 100k bps*/   

#define MASTER_SERIAL_WRITE  (2U)
#define MASTER_REG_WRITE     (4U)
#define MASTER_MEM_WRITE     (8U)

typedef struct 
{
   cy_scb_t* pSCB;
   uint8_t  u8RoleMode; /*I2C_MASTER_MODE / I2C_SLAVE_MODE*/
   uint8_t  u8SlaveAddress;
   uint32_t u32Timeout;
   cy_stc_scb_i2c_context_t tsContext;
}scb_i2c_t;

static scb_i2c_t tsI2C [CYHAL_NUM_OF_SCB] = {0U};

static scb_i2c_t* SCB_I2C_Get( uint8_t u8I2cGroup)
{
  scb_i2c_t* pI2C = NULL;
  cy_scb_t* pSCB = NULL;
  
  pSCB = CYHAL_SCB_Get(u8I2cGroup); /*Get SCB block*/
  
  if (NULL == pSCB )
  {
    pI2C = NULL;
  }
  else
  {
    pI2C = &(tsI2C[u8I2cGroup]);
    pI2C->pSCB = pSCB;
    pSCB->pI2CContext = &(pI2C->tsContext);
  }
  return pI2C;
}

static uint8_t SCB_I2C_Clock_Init(cy_scb_t* pSCB , i2c_config_t* tsConfig)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  uint32_t u32TargetFreq = tsConfig->u32ClockSpeed;
   
  if (DRIVER_TRUE == CYHAL_SCB_Clock_Init(pSCB,u32TargetFreq))
  {
    uint32_t u32DataRateHz = DATARATE_I2C_STANDARD_MODE;
    switch (tsConfig->u8SpeedMode)
    {
      case I2C_FAST_PLUS_MODE: /**< 1000k bps*/
        u32DataRateHz = DATARATE_I2C_FAST_PLUS_MODE;
        break;
      case I2C_FAST_MODE:      /**< 400k bps*/
        u32DataRateHz = DATARATE_I2C_FAST_MODE;
        break;
      case I2C_STANDARD_MODE: /**< 100k bps*/
        u32DataRateHz = DATARATE_I2C_STANDARD_MODE;
        break;
      default:
        u32DataRateHz = DATARATE_I2C_STANDARD_MODE;
        break;
    }   
    Cy_SCB_I2C_SetDataRate(pSCB->pBase, u32DataRateHz, u32TargetFreq);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

static uint8_t SCB_I2C_Config_Init( scb_i2c_t* pI2C , i2c_config_t* tsConfig)
{
  uint8_t u8HalResult    = DRIVER_FALSE;
  cy_en_scb_i2c_status_t eRetStatus = CY_SCB_I2C_BAD_PARAM;
  cy_stc_scb_i2c_config_t  tsI2C_config =
  {
    .i2cMode             = tsConfig->u8RoleMode,
    .useRxFifo           = true,
    .useTxFifo           = true,
    /*slave address use in I2C Slave mode*/
    .slaveAddress        = tsConfig->u8SlaveAddress,
    .slaveAddressMask    = tsConfig->u8SlaveAddress,
    .acceptAddrInFifo    = false,
    .ackGeneralAddr      = false,
    .enableWakeFromSleep = false
  };
  
  cy_scb_t* pSCB = pI2C->pSCB;
  
  if (tsConfig->u8RoleMode == I2C_SLAVE_MODE)
    tsI2C_config.ackGeneralAddr = true;
  else
    tsI2C_config.ackGeneralAddr = false;
  
  Cy_SCB_I2C_DeInit(pSCB->pBase);
  eRetStatus = Cy_SCB_I2C_Init(pSCB->pBase, &tsI2C_config, &(pI2C->tsContext));
  
  if (CY_SCB_I2C_SUCCESS == eRetStatus)
    u8HalResult = SCB_I2C_Clock_Init(pSCB,tsConfig);
  else
    u8HalResult = DRIVER_PARAMETER_ERROR;
  
  return u8HalResult;
}

static uint8_t SCB_I2C_PIN_Init(uint8_t u8I2cGroup, uint8_t u8SDA , uint8_t u8SCL)
{
  uint8_t u8HalSDAResult = DRIVER_FALSE;
  uint8_t u8HalSCLResult = DRIVER_FALSE;
  uint8_t u8HalResult    = DRIVER_FALSE;
  
  gpio_pin_config_t tsI2C_port_pin_cfg = 
  {
    .u8OutValue =  0U,                        /**< Pin output state */
    .u8DriveMode = CY_GPIO_DM_OD_DRIVESLOW,   /**< Drive mode */
    .u8HSIOM = HSIOM_SEL_ACT_6,               /**< HSIOM selection SCB[x].sda/scl*/
    .u8Vtrip = 0U,                            /**< Input buffer voltage trip type */
    .u8SlewRate = 0U,                         /**< Output buffer slew rate */
    .u8DriveStength = 0U,                     /**< Drive strength */
  };
  
  if (I2C_GROPU0 == u8I2cGroup)
    tsI2C_port_pin_cfg.u8HSIOM = HSIOM_SEL_DS_2;
  else
    tsI2C_port_pin_cfg.u8HSIOM = HSIOM_SEL_ACT_6;
  
  /* Port Configuration for I2C */
  u8HalSDAResult = HAL_GPIO_Pin_Init(u8SDA, &tsI2C_port_pin_cfg);
  u8HalSCLResult = HAL_GPIO_Pin_Init(u8SCL, &tsI2C_port_pin_cfg);
  if (DRIVER_TRUE == u8HalSDAResult && DRIVER_TRUE == u8HalSCLResult)
    u8HalResult = DRIVER_TRUE;
  else
    u8HalResult = DRIVER_PARAMETER_ERROR;
  
  return u8HalResult;
}

static cy_en_scb_i2c_status_t SCB_I2C_Master_SendStartBit( scb_i2c_t* pi2cMaster, uint8_t u8DevAddr, uint32_t u32BitRnW)
{
  cy_en_scb_i2c_status_t eSCB_status = CY_SCB_I2C_BAD_PARAM;
  
  if (NULL != pi2cMaster->pSCB)
  {
    if (CY_SCB_I2C_WRITE_XFER == u32BitRnW)
    {
      /* Make sure TX FIFO empty */
      for (uint32_t u32Timeout = 0 ; u32Timeout < pi2cMaster->u32Timeout ; u32Timeout++)
      {
        if (Cy_SCB_GetNumInTxFifo(pi2cMaster->pSCB->pBase) != 0ul)
          continue;
        else
        {
          eSCB_status = CY_SCB_I2C_SUCCESS;
          break;
        }
      } /*end of for*/
    }
    else
    {
      /* Make sure RX FIFO empty */
      for (uint32_t u32Timeout = 0 ; u32Timeout < pi2cMaster->u32Timeout ; u32Timeout++)
      {
        if (Cy_SCB_GetNumInRxFifo(pi2cMaster->pSCB->pBase) != 0ul)
          continue;
        else
        {
          eSCB_status = CY_SCB_I2C_SUCCESS;
          break;
        }
      } /*end of for*/
    }
    
    if (CY_SCB_I2C_SUCCESS == eSCB_status)
    {
      /* Send START and Receive ACK/NACK */
      eSCB_status = Cy_SCB_I2C_MasterSendStart(pi2cMaster->pSCB->pBase, 
                                               u8DevAddr, 
                                               u32BitRnW, 
                                               pi2cMaster->u32Timeout, 
                                               &(pi2cMaster->tsContext) );
    }
  }
  
  return eSCB_status;
}

static uint8_t SCB_I2C_Master_Transmit (uint8_t u8I2cGroup,uint8_t u8Mode,i2c_data_t * pTxData, uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_en_scb_i2c_status_t eSCB_status = CY_SCB_I2C_BAD_PARAM;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if ( (NULL != pI2C)    && 
       (NULL != pTxData) && 
       (NULL != pStatus) )
  {
    eSCB_status = SCB_I2C_Master_SendStartBit(pI2C,pTxData->u8SlaveAddress,CY_SCB_I2C_WRITE_XFER);
    *pStatus = eSCB_status;
    if (CY_SCB_I2C_SUCCESS == eSCB_status )
    {
      if (MASTER_REG_WRITE  == u8Mode)
      {
        Cy_SCB_I2C_MasterWriteByte(pI2C->pSCB->pBase,
                                   pTxData->u8RegAddress,
                                   pI2C->u32Timeout,
                                   &pI2C->tsContext);
      }
      else if (MASTER_MEM_WRITE == u8Mode)
      {
        Cy_SCB_I2C_MasterWriteByte(pI2C->pSCB->pBase,
                                   pTxData->u8MemAddressH,
                                   pI2C->u32Timeout,
                                   &pI2C->tsContext);
        
        Cy_SCB_I2C_MasterWriteByte(pI2C->pSCB->pBase,
                                   pTxData->u8MemAddressL,
                                   pI2C->u32Timeout,
                                   &pI2C->tsContext);
      }
      
      for (uint8_t i = 0u ; i < (pTxData->u8Length) ; i++)
      {
        eSCB_status = Cy_SCB_I2C_MasterWriteByte(pI2C->pSCB->pBase,
                                                 pTxData->pData[i],
                                                 pI2C->u32Timeout,
                                                 &pI2C->tsContext);
        if (CY_SCB_I2C_SUCCESS != eSCB_status)
        {
          *pStatus = eSCB_status;
          break;
        }
        else
          continue;
      } /* end of for*/
      
      if (CY_SCB_I2C_SUCCESS == eSCB_status)
      {
        /*Write Stop bit*/
        eSCB_status = Cy_SCB_I2C_MasterSendWriteStop(pI2C->pSCB->pBase, 
                                                     pI2C->u32Timeout, 
                                                     &(pI2C->tsContext) );
        if (CY_SCB_I2C_SUCCESS == eSCB_status)
        {
          u8HalResult = DRIVER_TRUE;
        }
        else
        {
          *pStatus = eSCB_status;
        }
      }
    }
    else
    {
      *pStatus = CY_SCB_I2C_MASTER_MANUAL_TIMEOUT;
    }
    
  }
  else
  {
    *pStatus = CY_SCB_I2C_BAD_PARAM;
  }
  
  return u8HalResult;
}

static uint8_t SCB_Master_Recv(scb_i2c_t* pI2C,uint8_t* pRxData,uint8_t u8Len,uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_en_scb_i2c_status_t eSCB_status = CY_SCB_I2C_BAD_PARAM;
  
  if (u8Len == 1u)
  {
    /*Read data & send NAK*/
    eSCB_status = Cy_SCB_I2C_MasterReadByte(pI2C->pSCB->pBase,
                                            CY_SCB_I2C_NAK,
                                            pRxData,
                                            pI2C->u32Timeout,
                                            &pI2C->tsContext);
  }
  else
  {
    for (uint8_t i = 0u; i < (u8Len-1u); i++)
    {
      /*Read data & send ACK*/
      eSCB_status = Cy_SCB_I2C_MasterReadByte(pI2C->pSCB->pBase,
                                              CY_SCB_I2C_ACK,
                                              pRxData,
                                              pI2C->u32Timeout,
                                              &pI2C->tsContext);
      pRxData++;
      if (CY_SCB_I2C_SUCCESS != eSCB_status)
      {
        *pStatus =  eSCB_status;
        break;
      }
      else
        continue;
    } // end of for
    
    /*Read last byte and send NACK*/
    if (CY_SCB_I2C_SUCCESS == eSCB_status)
    {
      eSCB_status = Cy_SCB_I2C_MasterReadByte(pI2C->pSCB->pBase,
                                              CY_SCB_I2C_NAK,
                                              pRxData,
                                              pI2C->u32Timeout,
                                              &pI2C->tsContext);
    }
  }
  
  if (CY_SCB_I2C_SUCCESS == eSCB_status)
  {
    Cy_SCB_I2C_MasterSendStop(pI2C->pSCB->pBase, 
                              pI2C->u32Timeout, 
                              &(pI2C->tsContext) );
    u8HalResult = DRIVER_TRUE;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Init( uint8_t u8I2cGroup, i2c_config_t* tsConfig)
{
  uint8_t u8HalResult = DRIVER_FALSE;  
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if ((NULL == pI2C) || (NULL == tsConfig))
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  else
  {
    pI2C->u8SlaveAddress = tsConfig->u8SlaveAddress;
    pI2C->u32Timeout     = tsConfig->u32Timeout;
    /*Init sda/scl pin*/
    if (DRIVER_TRUE == SCB_I2C_PIN_Init(u8I2cGroup,tsConfig->u8PinSDA,tsConfig->u8PinSCL))
    {
      /*Config I2C */
      u8HalResult = SCB_I2C_Config_Init(pI2C,tsConfig);
    }
    else
    {
      u8HalResult = DRIVER_PARAMETER_ERROR;
    }
  }
     
  return u8HalResult;
}

uint8_t HAL_I2C_DeInit (uint8_t u8I2cGroup)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  if (NULL == pI2C)
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  else
  {
    /*Disable I2C first*/
    if (DRIVER_TRUE == HAL_I2C_Active(u8I2cGroup,FUNC_DISABLE))
    {
      Cy_SCB_I2C_DeInit(pI2C->pSCB->pBase);
      u8HalResult = DRIVER_TRUE;
    }
    else
       u8HalResult = DRIVER_WRITE_ERROR;
  }
   
  return u8HalResult;
}

uint8_t HAL_I2C_Active( uint8_t u8I2cGroup, uint8_t u8Enable)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if (NULL != pI2C && (FUNC_ENABLE == u8Enable || FUNC_DISABLE == u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
      Cy_SCB_I2C_Enable(pI2C->pSCB->pBase);
    else
      Cy_SCB_I2C_Disable(pI2C->pSCB->pBase,&(pI2C->tsContext));
    
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

/************************************************************************************
* I2C MASTER MODE APIs
*************************************************************************************/
uint8_t HAL_I2C_Master_Start_Send(uint8_t u8I2cGroup,uint8_t u8Address,uint8_t u8ReadWrite)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if (NULL != pI2C && (I2C_ACTION_WRITE == u8ReadWrite || I2C_ACTION_READ == u8ReadWrite))
  {
    if (CY_SCB_I2C_SUCCESS == SCB_I2C_Master_SendStartBit(pI2C,u8Address,u8ReadWrite))
      u8HalResult = DRIVER_TRUE;
    else
      u8HalResult = DRIVER_TIMEOUT;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Master_ReStart_Send (uint8_t u8I2cGroup,uint8_t u8Address,uint8_t u8ReadWrite)
{
  uint8_t u8HalResult = DRIVER_TIMEOUT;
  cy_en_scb_i2c_status_t eSCB_status = CY_SCB_I2C_BAD_PARAM;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if (NULL != pI2C && (I2C_ACTION_WRITE == u8ReadWrite || I2C_ACTION_READ == u8ReadWrite))
  {
    eSCB_status = Cy_SCB_I2C_MasterSendReStart(pI2C->pSCB->pBase,
                                               u8Address,
                                               u8ReadWrite,
                                               pI2C->u32Timeout, 
                                               &(pI2C->tsContext) );
    if (CY_SCB_I2C_SUCCESS == eSCB_status)
      u8HalResult = DRIVER_TRUE;
    else
      u8HalResult = DRIVER_FALSE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
 
  return u8HalResult;
}

uint8_t HAL_I2C_Master_Stop_Send (uint8_t u8I2cGroup)
{
  uint8_t u8HalResult = DRIVER_TIMEOUT;
  cy_en_scb_i2c_status_t eSCB_status = CY_SCB_I2C_BAD_PARAM;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  if (NULL != pI2C)
  {
    /* Send STOP */
    eSCB_status = Cy_SCB_I2C_MasterSendStop(pI2C->pSCB->pBase,pI2C->u32Timeout,&(pI2C->tsContext));
    
    if (CY_SCB_I2C_SUCCESS == eSCB_status)
      u8HalResult = DRIVER_TRUE;
    else
      u8HalResult = DRIVER_FALSE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
 
  return u8HalResult;
}

uint8_t HAL_I2C_Master_DataByte_Write (uint8_t u8I2cGroup,uint8_t u8Data,uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_TIMEOUT;
  cy_en_scb_i2c_status_t eSCB_status = CY_SCB_I2C_BAD_PARAM;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if ((NULL != pI2C) && (NULL != pStatus ))
  {
    eSCB_status = Cy_SCB_I2C_MasterWriteByte(pI2C->pSCB->pBase,
                                             u8Data,
                                             pI2C->u32Timeout,
                                             &pI2C->tsContext);
    if (CY_SCB_I2C_SUCCESS == eSCB_status)
    {
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_FALSE;
    }
    *pStatus = eSCB_status;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Master_DataByte_Read (uint8_t u8I2cGroup,uint8_t* pRecvData,uint32_t u32AckNack,uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_TIMEOUT;
  cy_en_scb_i2c_status_t eSCB_status = CY_SCB_I2C_BAD_PARAM;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if ((NULL != pI2C) && (NULL != pRecvData) && (NULL != pStatus))
  {
    eSCB_status = Cy_SCB_I2C_MasterReadByte(pI2C->pSCB->pBase,
                                            u32AckNack,
                                            pRecvData,
                                            pI2C->u32Timeout,
                                            &pI2C->tsContext);
    if (CY_SCB_I2C_SUCCESS == eSCB_status)
    {
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_FALSE;
    }
    *pStatus = eSCB_status;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Master_Transmit(uint8_t u8I2cGroup,i2c_data_t* pTx, uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  
  if ((NULL != pTx) && (NULL != pStatus) )
  {
    u8HalResult = SCB_I2C_Master_Transmit(u8I2cGroup,MASTER_SERIAL_WRITE,pTx,pStatus);
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Master_Reg_Transmit (uint8_t u8I2cGroup,i2c_data_t* pTx, uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_FALSE;

  if ((NULL != pTx) && (NULL != pStatus) )
  {
    u8HalResult = SCB_I2C_Master_Transmit(u8I2cGroup,MASTER_REG_WRITE,pTx,pStatus);
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Master_Mem_Transmit (uint8_t u8I2cGroup,i2c_data_t* pTx, uint32_t* pStatus)
{
   uint8_t u8HalResult = DRIVER_FALSE;

  if ((NULL != pTx) && (NULL != pStatus) )
  {
    u8HalResult = SCB_I2C_Master_Transmit(u8I2cGroup,MASTER_MEM_WRITE,pTx,pStatus);
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Master_Receive (uint8_t u8I2cGroup,i2c_data_t* pRx, uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_FALSE; 
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  if ((NULL != pI2C)&& 
      (NULL != pRx) && 
      (NULL != pStatus) )
  {
    if (CY_SCB_I2C_SUCCESS == SCB_I2C_Master_SendStartBit(pI2C,pRx->u8SlaveAddress,CY_SCB_I2C_READ_XFER))
      u8HalResult = SCB_Master_Recv(pI2C,(pRx->pData),pRx->u8Length,pStatus);
    else
      u8HalResult = DRIVER_TIMEOUT;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Master_Reg_Receive (uint8_t u8I2cGroup,i2c_data_t* pRx, uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if ((NULL != pI2C)&& 
      (NULL != pRx) && 
      (NULL != pStatus) )
  {
    /* Send START and Receive ACK/NACK */
    if (DRIVER_TRUE == HAL_I2C_Master_Start_Send(u8I2cGroup,pRx->u8SlaveAddress,I2C_ACTION_WRITE))
    {
      /*Set register*/
      Cy_SCB_I2C_MasterWriteByte(pI2C->pSCB->pBase,
                                 pRx->u8RegAddress,
                                 pI2C->u32Timeout,
                                 &pI2C->tsContext);
      /*Send restart bit*/
      Cy_SCB_I2C_MasterSendReStart(pI2C->pSCB->pBase,
                                   pRx->u8SlaveAddress,
                                   CY_SCB_I2C_READ_XFER,
                                   pI2C->u32Timeout, 
                                   &(pI2C->tsContext) );
      /*Read data*/
      u8HalResult = SCB_Master_Recv(pI2C,(pRx->pData),pRx->u8Length,pStatus);
    }
    else
      u8HalResult = DRIVER_TIMEOUT;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  
  return u8HalResult;
}

uint8_t HAL_I2C_Master_Mem_Receive (uint8_t u8I2cGroup,i2c_data_t* pRx, uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_TIMEOUT;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  
  if ((NULL != pI2C)&& 
      (NULL != pRx) && 
      (NULL != pStatus) )
  {
    /* Send START and Receive ACK/NACK */
    if (DRIVER_TRUE == HAL_I2C_Master_Start_Send(u8I2cGroup,pRx->u8SlaveAddress,I2C_ACTION_WRITE))
    {
      /*Set memory address hi*/
      Cy_SCB_I2C_MasterWriteByte(pI2C->pSCB->pBase,
                                 pRx->u8MemAddressH,
                                 pI2C->u32Timeout,
                                 &pI2C->tsContext);
      
       /*Set memory address low*/
      Cy_SCB_I2C_MasterWriteByte(pI2C->pSCB->pBase,
                                 pRx->u8MemAddressL,
                                 pI2C->u32Timeout,
                                 &pI2C->tsContext);
      
      /*Send restart bit*/
      Cy_SCB_I2C_MasterSendReStart(pI2C->pSCB->pBase,
                                   pRx->u8SlaveAddress,
                                   CY_SCB_I2C_READ_XFER,
                                   pI2C->u32Timeout, 
                                   &(pI2C->tsContext) );
      /*Read data*/
      u8HalResult = SCB_Master_Recv(pI2C,(pRx->pData),pRx->u8Length,pStatus);
    }
    else
      u8HalResult = DRIVER_TIMEOUT;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  
  return u8HalResult;
}


/************************************************************************************
* I2C SLAVE MODE APIs
*************************************************************************************/
uint8_t HAL_I2C_Slave_ReadBuf_Config (uint8_t u8I2cGroup , uint8_t *pRdBuf, uint32_t u32Size)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  if ((NULL != pI2C) && (NULL != pRdBuf) && (u32Size > 0))
  {
    Cy_SCB_I2C_SlaveConfigReadBuf(pI2C->pSCB->pBase,pRdBuf,u32Size, &(pI2C->tsContext) );
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Slave_WriteBuf_Config (uint8_t u8I2cGroup , uint8_t *pWrBuf, uint32_t u32Size)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  if ((NULL != pI2C) && (NULL != pWrBuf) && (u32Size > 0))
  {
    Cy_SCB_I2C_SlaveConfigWriteBuf(pI2C->pSCB->pBase,pWrBuf,u32Size, &(pI2C->tsContext) );
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Slave_WriteTransferCount_Get (uint8_t u8I2cGroup , uint32_t* pRecv_Size)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  if ((NULL != pI2C) && (NULL != pRecv_Size))
  {
    *pRecv_Size  = Cy_SCB_I2C_SlaveGetWriteTransferCount(pI2C->pSCB->pBase, &(pI2C->tsContext));
    u8HalResult = DRIVER_TRUE;
  } 
  else
  {  
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}
         
uint8_t HAL_I2C_Slave_Status_Get(uint8_t u8I2cGroup,uint32_t* pStatus)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  if ((NULL != pI2C) && (NULL != pStatus))
  {
    *pStatus = Cy_SCB_I2C_SlaveGetStatus(pI2C->pSCB->pBase,&(pI2C->tsContext));
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Callback_Register(uint8_t u8I2cGroup , I2C_ISR pI2cCallback)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
  if ((DRIVER_TRUE == CYHAL_SCB_IRQ_Set(pI2C->pSCB,CYSCB_ROLE_I2C)) && (NULL != pI2cCallback ))
  {
    Cy_SCB_I2C_RegisterEventCallback(pI2C->pSCB->pBase, (scb_i2c_handle_events_t)pI2cCallback, &(pI2C->tsContext));
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_I2C_Slave_Interrupt_Set(uint8_t u8I2cGroup,uint8_t u8Enable, uint8_t u8Priority)
{
  UNUSED(u8Priority);
  
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_i2c_t* pI2C = SCB_I2C_Get(u8I2cGroup);
   
  if ((NULL != pI2C) && (FUNC_ENABLE == u8Enable || FUNC_DISABLE == u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
      Cy_SysInt_EnableIRQ(pI2C->pSCB->eIntr);
    else
      Cy_SysInt_DisableIRQ(pI2C->pSCB->eIntr);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

/*** End of file ***/

  