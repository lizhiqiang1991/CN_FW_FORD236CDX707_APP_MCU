/***************************************************************************
*\file hal_spi.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "cyhal_hwmgr.h"
#include "hal_gpio_def.h"
#include "hal_gpio.h"
#include "hal_spi_def.h"
#include "hal_spi.h"

#define SCB_SPI_OVERSAMPLING (16U)

typedef struct 
{
   cy_scb_t* pSCB;
   cy_stc_scb_spi_context_t tsContext;
}scb_spi_t;


static scb_spi_t tsSPI [CYHAL_NUM_OF_SCB] = {0U};
 
static const cy_stc_scb_spi_config_t tsSPIDefConfig =
{
    .spiMode                    = CY_SCB_SPI_MASTER,      /*** Specifies the mode of operation    ***/
    .subMode                    = CY_SCB_SPI_MOTOROLA,    /*** Specifies the submode of SPI operation    ***/
    .sclkMode                   = CY_SCB_SPI_CPHA0_CPOL0, /*** Clock is active low, data is changed on first edge ***/
    .oversample                 = SCB_SPI_OVERSAMPLING,   /*** SPI_CLOCK divided by SCB_SPI_OVERSAMPLING should be baudrate  ***/
    .rxDataWidth                = 16ul,                   /*** The width of RX data (valid range 4-16). It must be the same as \ref txDataWidth except in National sub-mode. ***/
    .txDataWidth                = 16ul,                   /*** The width of TX data (valid range 4-16). It must be the same as \ref rxDataWidth except in National sub-mode. ***/
    .enableMsbFirst             = true,                   /*** Enables the hardware to shift out the data element MSB first, otherwise, LSB first ***/
    .enableFreeRunSclk          = false,                  /*** Enables the master to generate a continuous SCLK regardless of whether there is data to send  ***/
    .enableInputFilter          = false,                  /*** Enables a digital 3-tap median filter to be applied to the input of the RX FIFO to filter glitches on the line. ***/
    .enableMisoLateSample       = true,                   /*** Enables the master to sample MISO line one half clock later to allow better timings. ***/
    .enableTransferSeperation   = true,                   /*** Enables the master to transmit each data element separated by a de-assertion of the slave select line (only applicable for the master mode) ***/
    .ssPolarity0                = false,                  /*** SS0: active low ***/
    .ssPolarity1                = false,                  /*** SS1: active low ***/
    .ssPolarity2                = false,                  /*** SS2: active low ***/
    .ssPolarity3                = false,                  /*** SS3: active low ***/
    .enableWakeFromSleep        = false,                  /*** When set, the slave will wake the device when the slave select line becomes active. Note that not all SCBs support this mode. Consult the device datasheet to determine which SCBs support wake from deep sleep. ***/
    // In case of using high level APIs, an user does not need to set these parameter. These parameter will be set in high level APIs such as "Cy_SCB_SPI_Transfer".
    .txFifoTriggerLevel         = 0ul,                    /*** Do not care ***/
    .rxFifoTriggerLevel         = 0ul,                    /*** Do not care ***/
    .rxFifoIntEnableMask        = 0ul,                    /*** Do not care  ***/
    .txFifoIntEnableMask        = 0ul,                    /*** Do not care  ***/
    .masterSlaveIntEnableMask   = 0ul,                    /*** Do not care  ***/
    .enableSpiDoneInterrupt     = false,
    .enableSpiBusErrorInterrupt = false,
};

static scb_spi_t* SCB_SPI_Get( uint8_t u8SPIGroup)
{
  scb_spi_t* pSPI = NULL;
  cy_scb_t* pSCB = NULL;
  
  pSCB = CYHAL_SCB_Get(u8SPIGroup); /*Get SCB block*/
  
  if (NULL == pSCB )
  {
    pSPI = NULL;
  }
  else
  {
    pSPI = &(tsSPI[u8SPIGroup]);
    pSPI->pSCB = pSCB;
    pSCB->pSPIContext = &(pSPI->tsContext);
  }
  return pSPI;
}

static uint8_t SCB_SPI_PIN_Init( uint8_t u8SPIGroup, spi_config_t* tsConfig)
{
  /* Master Settings 
  SCB_MISO_DRIVE_MODE CY_GPIO_DM_HIGHZ
  SCB_MOSI_DRIVE_MODE CY_GPIO_DM_STRONG_IN_OFF
  SCB_CLK_DRIVE_MODE  CY_GPIO_DM_STRONG_IN_OFF
  SCB_SEL0_DRIVE_MODE CY_GPIO_DM_STRONG_IN_OFF
  */
  
  /* Slave Settings 
  SCB_MISO_DRIVE_MODE CY_GPIO_DM_STRONG_IN_OFF
  SCB_MOSI_DRIVE_MODE CY_GPIO_DM_HIGHZ
  SCB_CLK_DRIVE_MODE  CY_GPIO_DM_HIGHZ
  SCB_SEL0_DRIVE_MODE CY_GPIO_DM_HIGHZ
  */
  
  uint32_t u32HalResult = 0x00000000u;
  uint8_t  u8Result = DRIVER_FALSE;
  uint8_t u8MISO = tsConfig->u8MISO_pin; 
  uint8_t u8MOSI = tsConfig->u8MOSI_pin; 
  uint8_t u8Clk  = tsConfig->u8CLK_Pin;
  uint8_t u8Sel  = tsConfig->u8CS_Pin;
  
  gpio_pin_config_t SPI_port_pin_cfg = 
  {
    .u8OutValue =  0U,               /**< Pin output state */
    .u8DriveMode = 0U,               /**< Drive mode will be updated in runtime */
    .u8HSIOM = HSIOM_SEL_GPIO,      /**< HSIOM selection SCB[x].tx/rx/cts/rts */
    .u8Vtrip = 0U,                   /**< Input buffer voltage trip type */
    .u8SlewRate = 0U,                /**< Output buffer slew rate */
    .u8DriveStength = 0U,            /**< Drive strength */
  };
  
  if (SPI_GROPU0 == u8SPIGroup)
    SPI_port_pin_cfg.u8HSIOM = HSIOM_SEL_DS_6;
  else
    SPI_port_pin_cfg.u8HSIOM = HSIOM_SEL_ACT_7;
  
  if (IO_NC != u8Sel)  /* Port Configuration for spi SEL0 */
  {
    if (SPI_MASTER == tsConfig->u32Mode)
      SPI_port_pin_cfg.u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF;
    else
      SPI_port_pin_cfg.u8DriveMode = CY_GPIO_DM_HIGHZ;
    u32HalResult |= HAL_GPIO_Pin_Init(u8Sel, &SPI_port_pin_cfg);
    u32HalResult <<= 8u;
  }
  
  if (IO_NC != u8Clk)  /* Port Configuration for spi clk   */
  {
    if (SPI_MASTER == tsConfig->u32Mode)
      SPI_port_pin_cfg.u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF;
    else
      SPI_port_pin_cfg.u8DriveMode = CY_GPIO_DM_HIGHZ;
    u32HalResult |= HAL_GPIO_Pin_Init(u8Clk, &SPI_port_pin_cfg);
    u32HalResult <<= 8u;
  }
  
  if (IO_NC != u8MOSI)  /* Port Configuration for MOSI*/
  {
    if (SPI_MASTER == tsConfig->u32Mode)
      SPI_port_pin_cfg.u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF;
    else
      SPI_port_pin_cfg.u8DriveMode = CY_GPIO_DM_HIGHZ;
    u32HalResult |= HAL_GPIO_Pin_Init(u8MOSI, &SPI_port_pin_cfg);
    u32HalResult <<= 8u;
  }
  
  if (IO_NC != u8MISO)  /* Port Configuration for MISO */
  {
    if (SPI_MASTER == tsConfig->u32Mode)
      SPI_port_pin_cfg.u8DriveMode = CY_GPIO_DM_HIGHZ;
    else
      SPI_port_pin_cfg.u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF;
    u32HalResult |= HAL_GPIO_Pin_Init(u8MISO, &SPI_port_pin_cfg);
    u32HalResult <<= 8u;
  }
  
  if ((u32HalResult & 0x00010101u) || (u32HalResult & 0x01010101u))
    u8Result = DRIVER_TRUE;
  else
    u8Result = DRIVER_WRITE_ERROR;
  
  return u8Result;
}

static uint8_t SCB_SPI_Clock_Init( cy_scb_t* pSCB , uint32_t u32BaudRate)
{
  uint32_t u32TargetFreq = SCB_SPI_OVERSAMPLING * u32BaudRate;;
  return CYHAL_SCB_Clock_Init(pSCB,u32TargetFreq);
}

static uint8_t SCB_SPI_Config_Init( scb_spi_t* pSPI , spi_config_t* tsConfig)
{
  cy_en_scb_spi_status_t eRetStatus = CY_SCB_SPI_BAD_PARAM;
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_stc_scb_spi_config_t tsSCB_SPI_cfg;
  
  /* Initialize SPI Config */
  memcpy(&tsSCB_SPI_cfg,&tsSPIDefConfig,sizeof(cy_stc_scb_spi_config_t));
  tsSCB_SPI_cfg.spiMode         = tsConfig->u32Mode;           /**< SPI master/ slave*/
  tsSCB_SPI_cfg.subMode         = tsConfig->u32SubMode;        /**< Specifies the sub mode of SPI operation    ***/
  tsSCB_SPI_cfg.sclkMode        = tsConfig->u32SclkMode;       /**< Clock is active low, data is changed on first edge ***/
  tsSCB_SPI_cfg.rxDataWidth     = tsConfig->u32DataWidth;      /**< The width of RX/TX data (valid range 4-16).*/
  tsSCB_SPI_cfg.txDataWidth     = tsConfig->u32DataWidth;
  tsSCB_SPI_cfg.enableMsbFirst  = tsConfig->u8EnableMsbFirst;  /*** Enables the hardware to shift out the data element MSB first, otherwise, LSB first ***/
  /**   De-initialization for peripherals     */
  Cy_SCB_SPI_DeInit(pSPI->pSCB->pBase);
  /********************************************/
  /* SCB initialization for SPI communication */
  /********************************************/
  eRetStatus = Cy_SCB_SPI_Init(pSPI->pSCB->pBase, &tsSCB_SPI_cfg,&(pSPI->tsContext));
  if (CY_SCB_SPI_SUCCESS == eRetStatus)
  {
    u8HalResult = SCB_SPI_Clock_Init(pSPI->pSCB,tsConfig->u32BaudRate);
    /*******************************************/
    /* Interrupt setting for SPI communication */
    /******************************************
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, irqSCB);
    NVIC_EnableIRQ(irq_cfg.intIdx);
    */
  }
  else
    u8HalResult = DRIVER_PARAMETER_ERROR;
  return u8HalResult;
}

uint8_t HAL_SPI_Init (uint8_t u8SPIGroup, spi_config_t* tsConfig)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_spi_t* pSPI = SCB_SPI_Get(u8SPIGroup);
  if ((NULL == pSPI) || (NULL == tsConfig))
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  else
  {
    /*Init MISO/MOSI/CLK/SEL0 pin*/
    if (DRIVER_TRUE == SCB_SPI_PIN_Init(u8SPIGroup,tsConfig))
    {
      /*Config SPI */
      u8HalResult = SCB_SPI_Config_Init(pSPI,tsConfig);
    }
    else
    {
      u8HalResult = DRIVER_WRITE_ERROR;
    }
  }
  return u8HalResult;
}

uint8_t HAL_SPI_DeInit (uint8_t u8SPIGroup) 
{ 
  uint8_t u8HalResult = DRIVER_FALSE;
  scb_spi_t* pSPI = SCB_SPI_Get(u8SPIGroup);
  if (NULL == pSPI)
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  else
  {
    Cy_SCB_SPI_DeInit(pSPI->pSCB->pBase);
    u8HalResult = DRIVER_TRUE;
  }     
  return u8HalResult;
}

uint8_t HAL_SPI_Active (uint8_t u8SPIGroup,uint8_t u8Enable)
{
  uint8_t u8HalResult  = DRIVER_FALSE;
  scb_spi_t* pSPI = SCB_SPI_Get(u8SPIGroup);
  
  if (NULL != pSPI && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
    {
      Cy_SCB_SPI_Enable(pSPI->pSCB->pBase);
    }
    else
    {
      Cy_SCB_SPI_Disable(pSPI->pSCB->pBase,&(pSPI->tsContext));
    }
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

 
/*** End of file ***/

  