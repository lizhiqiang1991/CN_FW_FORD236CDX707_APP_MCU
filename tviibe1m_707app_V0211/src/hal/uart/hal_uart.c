/***************************************************************************
*\file hal_uart.c 
****************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "cyhal_hwmgr.h"
#include "hal_gpio_def.h"
#include "hal_gpio.h"
#include "hal_uart_def.h"
#include "hal_uart.h"

/*For uart printf*/
#define MAX_TEXT_LEN  (128u)
#define PRINTF_TIMEOUT (10u) /*10 mSec*/
static char u8PrintfTextBuf[MAX_TEXT_LEN + 8u];
 
/* User Local Definition */
#define UART_OVERSAMPLING (8ul)

static const cy_stc_scb_uart_config_t tsUartDefConfig =
{
    .uartMode                   = CY_SCB_UART_STANDARD,
    .oversample                 = UART_OVERSAMPLING,
    .dataWidth                  = 8ul,
    .enableMsbFirst             = false,
    .stopBits                   = CY_SCB_UART_STOP_BITS_1,
    .parity                     = CY_SCB_UART_PARITY_NONE,
    .enableInputFilter          = false,
    .dropOnParityError          = false,
    .dropOnFrameError           = false,
    .enableMutliProcessorMode   = false,
    .receiverAddress            = 0ul,
    .receiverAddressMask        = 0ul,
    .acceptAddrInFifo           = false,
    .irdaInvertRx               = false,
    .irdaEnableLowPowerReceiver = false,
    .smartCardRetryOnNack       = false,
    .enableCts                  = false,
    .ctsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .rtsRxFifoLevel             = 0ul,
    .rtsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .breakWidth                 = 0ul,
    .rxFifoTriggerLevel         = 0ul,
    .rxFifoIntEnableMask        = 0ul,
    .txFifoTriggerLevel         = 0ul,
    .txFifoIntEnableMask        = 0ul
};

typedef struct 
{
    cy_scb_t* pSCB;
    cy_stc_scb_uart_context_t tsContext;
}scb_uart_t;

static scb_uart_t tsUart [CYHAL_NUM_OF_SCB] = {0U};

static scb_uart_t* SCB_Uart_Get( uint8_t u8UartGroup)
{
    scb_uart_t* pUart = NULL;
    cy_scb_t* pSCB = NULL;
    
    pSCB = CYHAL_SCB_Get(u8UartGroup); /*Get SCB block*/
    
    if (NULL == pSCB )
    {
        pUart = NULL;
    }
    else
    {
        pUart = &(tsUart[u8UartGroup]);
        pUart->pSCB = pSCB;
        pSCB->pUartContext = &(pUart->tsContext);
    }
    return pUart;
}

static uint8_t SCB_Uart_PIN_Init(uart_config_t* tsConfig)
{
    uint32_t u32HalResult = 0x00000000U;
    uint8_t  u8Result = DRIVER_FALSE;
    
    uint8_t u8Tx  = tsConfig->u8TxPin;
    uint8_t u8Rx  = tsConfig->u8RxPin;
    uint8_t u8Cts = tsConfig->u8CtsPin;
    uint8_t u8Rts = tsConfig->u8RtsPin;
    
    gpio_pin_config_t tsUart_port_pin_cfg = 
    {
        .u8OutValue =  0U,               /**< Pin output state */
        .u8DriveMode = CY_GPIO_DM_HIGHZ, /**< Drive mode */
        .u8HSIOM = HSIOM_SEL_ACT_5,      /**< HSIOM selection SCB[x].tx/rx/cts/rts */
        .u8Vtrip = 0U,                   /**< Input buffer voltage trip type */
        .u8SlewRate = 0U,                /**< Output buffer slew rate */
        .u8DriveStength = 0U,            /**< Drive strength */
    };
    
    if (IO_NC != u8Cts)  /* Port Configuration for UART CTS (input) */
    {
        tsUart_port_pin_cfg.u8DriveMode = CY_GPIO_DM_HIGHZ;
        u32HalResult |= HAL_GPIO_Pin_Init(u8Cts, &tsUart_port_pin_cfg);
        u32HalResult <<= 8u;
    }
    
    if (IO_NC != u8Rts)  /* Port Configuration for UART RTS (output) */
    {
        tsUart_port_pin_cfg.u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF;
        u32HalResult |= HAL_GPIO_Pin_Init(u8Rts, &tsUart_port_pin_cfg);
        u32HalResult <<= 8u;
    }
    
    if (IO_NC != u8Rx)  /* Port Configuration for UART Rx (input)*/
    {
        tsUart_port_pin_cfg.u8DriveMode = CY_GPIO_DM_HIGHZ;
        u32HalResult |= HAL_GPIO_Pin_Init(u8Rx, &tsUart_port_pin_cfg);
        u32HalResult <<= 8u;
    }
    
    if (IO_NC != u8Tx)  /* Port Configuration for UART Tx (output)*/
    {
        tsUart_port_pin_cfg.u8DriveMode = CY_GPIO_DM_STRONG_IN_OFF;
        u32HalResult |= HAL_GPIO_Pin_Init(u8Tx, &tsUart_port_pin_cfg);
    }
    
    /*if ((u32HalResult & 0x00000101u) || (u32HalResult & 0x01010101u))*/
    if (u32HalResult & 0x00000101u)
        u8Result = DRIVER_TRUE;
    else
        u8Result = DRIVER_WRITE_ERROR;
    
    return u8Result;
}

static uint8_t SCB_Uart_Clock_Init( cy_scb_t* pSCB , uint32_t u32BaudRate)
{
    uint32_t u32TargetFreq = UART_OVERSAMPLING * u32BaudRate;
    return CYHAL_SCB_Clock_Init(pSCB,u32TargetFreq);
}

static uint8_t SCB_Uart_Config_Init( scb_uart_t* pUart , uart_config_t* tsConfig)
{
    uint8_t u8HalResult;
    cy_en_scb_uart_status_t eRetStatus = CY_SCB_UART_BAD_PARAM;
    cy_stc_scb_uart_config_t ts_scb_uart_cfg = {0u};
    /* Initialize SCB UART*/
    memcpy(&ts_scb_uart_cfg,&tsUartDefConfig,sizeof(cy_stc_scb_uart_config_t));
    
    ts_scb_uart_cfg.dataWidth = tsConfig->u8DataBits;
    ts_scb_uart_cfg.parity    = tsConfig->u8Parity;
    ts_scb_uart_cfg.stopBits  = tsConfig->u8StopBits;
    
    Cy_SCB_UART_DeInit(pUart->pSCB->pBase);
    eRetStatus = Cy_SCB_UART_Init (pUart->pSCB->pBase, &ts_scb_uart_cfg, &(pUart->tsContext));
    if (CY_SCB_UART_SUCCESS == eRetStatus)
        u8HalResult = SCB_Uart_Clock_Init(pUart->pSCB,tsConfig->u32BaudRate);
    else
        u8HalResult = DRIVER_PARAMETER_ERROR;
    
    return u8HalResult;
}

uint8_t HAL_Uart_Init (uint8_t u8UartGroup, uart_config_t* tsConfig) 
{ 
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((NULL == pUart) || (NULL == tsConfig))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        /*Init tx/rx/cts/rts pin*/
        if (DRIVER_TRUE == SCB_Uart_PIN_Init(tsConfig))
        {
            /*Config Uart */
            u8HalResult = SCB_Uart_Config_Init(pUart,tsConfig);
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }
    return u8HalResult;
} 

uint8_t HAL_Uart_DeInit (uint8_t u8UartGroup) 
{ 
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if (NULL == pUart)
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        Cy_SCB_UART_DeInit(pUart->pSCB->pBase);
        u8HalResult = DRIVER_TRUE;
    }     
    return u8HalResult;
} 

uint8_t HAL_Uart_Active (uint8_t u8UartGroup,uint8_t u8Enable) 
{ 
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    
    if (NULL != pUart && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
    {
        if (FUNC_ENABLE == u8Enable)
        {
            Cy_SCB_UART_Enable(pUart->pSCB->pBase);
        }
        else
        {
            Cy_SCB_UART_Disable(pUart->pSCB->pBase,&(pUart->tsContext));
        }
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
} 

uint8_t HAL_Uart_Write (uint8_t u8UartGroup,uint8_t* pBuf,uint16_t u16Len) 
{ 
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((NULL == pUart) || (NULL == pBuf) || (u16Len == 0))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        Cy_SCB_WriteArray(pUart->pSCB->pBase, pBuf, u16Len);
        u8HalResult = DRIVER_TRUE;
    }
    return u8HalResult;
} 

uint8_t HAL_Uart_Read (uint8_t u8UartGroup,uint8_t* pBuf,uint16_t u16Len) 
{ 
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((NULL == pUart) || (NULL == pBuf) || (u16Len == 0))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        Cy_SCB_UART_GetArray(pUart->pSCB->pBase, pBuf, u16Len);
        u8HalResult = DRIVER_TRUE;
    }
    return u8HalResult;
} 

uint8_t HAL_Uart_Getc (uint8_t u8UartGroup,uint8_t* pu8Char )
{
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((NULL == pUart) || (NULL == pu8Char))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        *pu8Char = Cy_SCB_UART_Get(pUart->pSCB->pBase);
        u8HalResult = DRIVER_TRUE;
    }
    
    return u8HalResult;
}

uint8_t HAL_Uart_Putc (uint8_t u8UartGroup,uint8_t u8Value)
{
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if (NULL == pUart)
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        Cy_SCB_UART_Put(pUart->pSCB->pBase,u8Value);
        u8HalResult = DRIVER_TRUE;
    }
    
    return u8HalResult;
}

uint8_t HAL_Uart_IsTxComplete (uint8_t u8UartGroup,bool* pTxComplete)
{
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((NULL == pUart) || (NULL == pTxComplete))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        *pTxComplete = Cy_SCB_UART_IsTxComplete(pUart->pSCB->pBase);
        u8HalResult = DRIVER_TRUE;
    }
    
    return u8HalResult;
}

uint8_t HAL_Uart_GetNumInRxFifo (uint8_t u8UartGroup,uint16_t* pu16Rxnum)
{
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((NULL == pUart) || (NULL == pu16Rxnum))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        *pu16Rxnum = Cy_SCB_UART_GetNumInRxFifo(pUart->pSCB->pBase);
        u8HalResult = DRIVER_TRUE;
    }
    
    return u8HalResult;
}

uint8_t HAL_Uart_GetRxStatus(uint8_t u8UartGroup,uint32_t* pRxStatus)
{
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((NULL == pUart) || (NULL == pRxStatus))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        *pRxStatus = Cy_SCB_UART_GetReceiveStatus(pUart->pSCB->pBase,&pUart->tsContext);
        u8HalResult = DRIVER_TRUE;
    }
    
    return u8HalResult;
}

uint8_t HAL_Uart_GetTxStatus(uint8_t u8UartGroup,uint32_t* pTxStatus)
{
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((NULL == pUart) || (NULL == pTxStatus))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        //*pTxStatus = Cy_SCB_UART_GetTransmitStatus(pUart->pSCB->pBase,&pUart->tsContext);
        *pTxStatus = Cy_SCB_UART_GetTxFifoStatus(pUart->pSCB->pBase);
        u8HalResult = DRIVER_TRUE;
    }
    
    return u8HalResult;
}

uint8_t HAL_Uart_Interrupt_Set(uint8_t u8UartGroup, uint8_t u8Enable, uint8_t u8Priority)
{
    uint8_t u8HalResult;
    
    UNUSED(u8Priority);
    
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    
    if (NULL != pUart  && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
    {
         
        if (FUNC_ENABLE == u8Enable)
        {
            CYHAL_SCB_IRQ_Set(pUart->pSCB,CYSCB_ROLE_UART);
        }
        else
        {
            Cy_SysInt_DisableIRQ(pUart->pSCB->eIntr);
        }
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    
    return u8HalResult;
}

uint8_t HAL_Uart_Callback_Register (uint8_t u8UartGroup, UART_ISR pUartCallback) 
{ 
    uint8_t u8HalResult;
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    if ((DRIVER_TRUE == CYHAL_SCB_IRQ_Set(pUart->pSCB,CYSCB_ROLE_UART)) && (NULL != pUartCallback ))
    {
        Cy_SCB_UART_RegisterCallback(pUart->pSCB->pBase, (scb_uart_handle_events_t)pUartCallback,  &(pUart->tsContext));
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
} 

uint8_t HAL_UartPrintf(uint8_t u8UartGroup,void *fmt, ...)
{
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    uint8_t u8HalResult;
    uint16_t u16Timeout = 0;
    
    va_list arg;
    if (NULL != pUart)
    {
        /* UART Print */
        va_start(arg, fmt);
        
        vsnprintf (u8PrintfTextBuf, MAX_TEXT_LEN, (const char *) fmt, arg );
        
        for ( u16Timeout = 0 ; u16Timeout <= PRINTF_TIMEOUT ; u16Timeout++)
        {
            if (true == Cy_SCB_UART_IsTxComplete(pUart->pSCB->pBase))
                break;
            else
                Cy_SysLib_Delay(1);
        }
        
        if (u16Timeout >= PRINTF_TIMEOUT)
        {
            u8HalResult = DRIVER_TIMEOUT;
        }
        else
        { 
            Cy_SCB_UART_PutString(pUart->pSCB->pBase, (char_t const*)u8PrintfTextBuf);;
            u8HalResult = DRIVER_TRUE;
        }
        va_end(arg);
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}


void UART_RECV(uint8_t u8UartGroup, char* buf,uint16_t len)
{
    scb_uart_t* pUart = SCB_Uart_Get(u8UartGroup);
    
    /* Start receiving */
    Cy_SCB_UART_Receive(pUart->pSCB->pBase, buf, len, &(pUart->tsContext));
}

/*** End of file ***/

  