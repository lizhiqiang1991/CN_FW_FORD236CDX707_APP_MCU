#ifndef CYHAL_HARDWARE_MANAGER
#define CYHAL_HARDWARE_MANAGER

typedef void(*CYHAL_TCPWM_ISR) (void *pContext);

#define CYSCB_ROLE_UART         (0x02U)
#define CYSCB_ROLE_I2C          (0x04U)
#define CYSCB_ROLE_SPI          (0x06U)

typedef struct 
{
  volatile stc_SCB_t* pBase;
  en_clk_dst_t eClkSrc;
  cy_en_intr_t eIntr;
  cy_stc_scb_i2c_context_t*   pI2CContext;
  cy_stc_scb_uart_context_t*  pUartContext;
  cy_stc_scb_spi_context_t*   pSPIContext;
  uint8_t u8SCBRole;
}cy_scb_t;

typedef struct {
  volatile stc_TCPWM_GRP_CNT_t* pBase;
  const en_clk_dst_t eClkSrc;
  const cy_en_intr_t eIntr;
  CYHAL_TCPWM_ISR   pCallback;
  void*       pContext;          /*ISR Context*/
  uint8_t u8IsISREnable;         /*Is ISR enable*/ 
}cy_tcpwm_t;

cy_scb_t* CYHAL_SCB_Get( uint8_t u8SCB_Num);
cy_tcpwm_t* CYHAL_TCPWM_Get( uint8_t u8TCPWM_Num);

uint8_t CYHAL_TCPWM_IRQ_Callback_Set(uint8_t u8TCPWM_Num, CYHAL_TCPWM_ISR pCallback, void* pContext);
uint8_t CYHAL_SCB_IRQ_Set(cy_scb_t* pSCB,uint8_t u8SCBRole);

uint8_t CYHAL_SCB_Divider24_5_Assign(const cy_scb_t* ptsSCB,uint64_t u64TargetFreq, uint64_t u64SourceFreq);
uint8_t CYHAL_SCB_Clock_Init(cy_scb_t* ptsSCB ,uint32_t u32TargetFreq);

#endif

