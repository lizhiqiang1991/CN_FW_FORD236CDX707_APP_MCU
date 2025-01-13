/***************************************************************************
*\file hal_gpio.c
****************************************************************************/ 

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_gpio_def.h"
#include "hal_gpio.h"


#define MAPPING_PORT_PIN(u32Pin)  uint8_t u8PortNum = CYHAL_GET_PORT((u32Pin));\
                                  uint8_t u8Pin  = CYHAL_GET_PIN((u32Pin)); \
                                  volatile stc_GPIO_PRT_t* pPort = HAL_GPIO_PortToAddr(u8PortNum)

#define INVALID_GPIO ((volatile stc_GPIO_t*) 0xffffffffu)

static stc_GPIO_PRT_t* HAL_GPIO_PortToAddr(uint32_t u32PortNum)
{
    stc_GPIO_PRT_t* base;

    if(u32PortNum < (uint32_t)IOSS_GPIO_GPIO_PORT_NR)
    {
        base = (stc_GPIO_PRT_t *)(GPIO_BASE + (GPIO_PRT_SECTION_SIZE * u32PortNum));
    }
    else
    {
        /* Error: Return invalid base address */
        base = (stc_GPIO_PRT_t *)(INVALID_GPIO);
    }

    return (base);
}

/* Port operation*/
uint8_t HAL_GPIO_Port_Init(uint32_t u32PortGroup, gpio_port_config_t *tsConfig)
{
    uint8_t u8HalResult;
    cy_stc_gpio_prt_config_t tdPortCfg;

    if(NULL != tsConfig )
    {
        stc_GPIO_PRT_t* pBase = HAL_GPIO_PortToAddr(u32PortGroup);

        if ((stc_GPIO_PRT_t *)INVALID_GPIO != pBase)
        {
            memset(&tdPortCfg,0,sizeof(tdPortCfg));

            tdPortCfg.out        = tsConfig->u32OutValue;      /**< Initial output data for the IO pins in the port */
            tdPortCfg.cfg        = tsConfig->u32DriveMode;     /**< Port pin drive modes and input buffer enable configuration */
            tdPortCfg.cfgIn      = tsConfig->u32CfgIn;         /**< Port pin input buffer configuration */
            tdPortCfg.cfgOut     = tsConfig->u32CfgOut;        /**< Port pin output buffer configuration */
            tdPortCfg.sel0Active = tsConfig->u32HSIOM0;        /**< HSIOM selection for port pins 0,1,2,3 */
            tdPortCfg.sel1Active = tsConfig->u32HSIOM1;        /**< HSIOM selection for port pins 4,5,6,7 */

            if (CY_GPIO_SUCCESS == Cy_GPIO_Port_Init(pBase, &tdPortCfg))
            {
                u8HalResult = DRIVER_TRUE;
            }
            else
            {
                u8HalResult = DRIVER_REGISTER_ERROR;
            }
        }
        else
        {
          u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }

    return u8HalResult;
}

uint8_t HAL_GPIO_Port_DeInit(uint32_t u32PortGroup)
{
    uint8_t u8HalResult;
    stc_GPIO_PRT_t* pBase = HAL_GPIO_PortToAddr(u32PortGroup);

    if ((stc_GPIO_PRT_t *)INVALID_GPIO != pBase)
    {
        Cy_GPIO_Port_Deinit(pBase);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        UNUSED(pBase);
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }

    return u8HalResult;
}

/* Read the hardware pin state of masked pins in the specified group */
uint8_t HAL_GPIO_PortRead(uint32_t u32PortGroup,uint32_t u32Mask, uint32_t * pReturn)
{
    uint8_t u8HalResult;
    stc_GPIO_PRT_t* pBase = HAL_GPIO_PortToAddr(u32PortGroup);

    if (((stc_GPIO_PRT_t*)INVALID_GPIO != pBase) && (NULL != pReturn ))
    {
        *pReturn = u32Mask & pBase->unIN.u32Register;
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        UNUSED(pBase);
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

/* Write the value contained in the value parameter to the port group.  */
uint8_t HAL_GPIO_PortWrite(uint32_t u32PortGroup,uint32_t u32Mask, uint32_t u32PortValue)
{
    uint8_t u8HalResult;
    stc_GPIO_PRT_t* pBase = HAL_GPIO_PortToAddr(u32PortGroup);
  
    if ((stc_GPIO_PRT_t*)INVALID_GPIO != pBase)
    {
        pBase->unOUT.u32Register = (u32PortValue & u32Mask);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        UNUSED(pBase);
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }

    return u8HalResult;
}

uint8_t HAL_GPIO_PortToggle(uint32_t u32PortGroup,uint32_t u32Mask)
{
    uint8_t u8HalResult;
    stc_GPIO_PRT_t* pBase = HAL_GPIO_PortToAddr(u32PortGroup);

    if ((stc_GPIO_PRT_t*)INVALID_GPIO != pBase)
    {
        pBase->unOUT_INV.u32Register = u32Mask;
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        UNUSED(pBase);
        u8HalResult = DRIVER_PARAMETER_ERROR; 
    }

    return u8HalResult;
}

static uint8_t HAL_GPIO_PinConfig_Check( gpio_pin_config_t *tsConfig)
{
    uint8_t u8HalResult;

    if((tsConfig->u8OutValue == PIN_LOW) || (tsConfig->u8OutValue == PIN_HIGH))
    {
        if( (tsConfig->u8DriveMode == CY_GPIO_DM_ANALOG)                ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLUP_IN_OFF)         ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLDOWN_IN_OFF)       ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_OD_DRIVESLOW_IN_OFF)   ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_OD_DRIVESHIGH_IN_OFF)  ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_STRONG_IN_OFF)         ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLUP_DOWN_IN_OFF)    ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_HIGHZ)                 ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLUP)                ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLDOWN)              ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_OD_DRIVESLOW)          ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_OD_DRIVESHIGH)         ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_STRONG)                ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLUP_DOWN) )
        {
            if( (tsConfig->u8Vtrip == CY_GPIO_VTRIP_CMOS) ||\
                (tsConfig->u8Vtrip == CY_GPIO_VTRIP_TTL) )
            {
                if( (tsConfig->u8SlewRate == CY_GPIO_SLEW_FAST)||\
                    (tsConfig->u8SlewRate == CY_GPIO_SLEW_SLOW) )
                {
                    if( (tsConfig->u8DriveStength == CY_GPIO_DRIVE_FULL)||\
                        (tsConfig->u8DriveStength == CY_GPIO_DRIVE_1_2) ||\
                        (tsConfig->u8DriveStength == CY_GPIO_DRIVE_1_4) )
                    {
                        u8HalResult = DRIVER_TRUE;
                    }
                    else
                    {
                        u8HalResult = DRIVER_PARAMETER_ERROR;
                    }
                }
                else
                {
                    u8HalResult = DRIVER_PARAMETER_ERROR;
                }
            }
            else
            {
                u8HalResult = DRIVER_PARAMETER_ERROR;
            }
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    
    return u8HalResult;
}

/* Pin operation*/
uint8_t HAL_GPIO_Pin_Init(uint8_t u8PinNum, gpio_pin_config_t *tsConfig)
{
    uint8_t u8HalResult;
    cy_stc_gpio_pin_config_t tdPortPinCfg;
  
    if(NULL != tsConfig)
    {
        if( DRIVER_TRUE == HAL_GPIO_PinConfig_Check(tsConfig))
        {
            MAPPING_PORT_PIN(u8PinNum);

            if ((stc_GPIO_PRT_t *)INVALID_GPIO != pPort)
            {
                memset(&tdPortPinCfg,0,sizeof(tdPortPinCfg));

                tdPortPinCfg.driveMode = CY_GPIO_DM_ANALOG;
                tdPortPinCfg.hsiom     = HSIOM_SEL_GPIO;

                tdPortPinCfg.outVal    = tsConfig->u8OutValue;              /**< Pin output state */
                tdPortPinCfg.driveMode = tsConfig->u8DriveMode;             /**< Drive mode */       
                tdPortPinCfg.hsiom     = (en_hsiom_sel_t)tsConfig->u8HSIOM; /**< HSIOM selection */
                tdPortPinCfg.vtrip     = tsConfig->u8Vtrip;                 /**< Input buffer voltage trip type */
                tdPortPinCfg.slewRate  = tsConfig->u8SlewRate;              /**< Output buffer slew rate */
                tdPortPinCfg.driveSel  = tsConfig->u8DriveStength;          /**< Drive strength */
        
                if (CY_GPIO_SUCCESS == Cy_GPIO_Pin_Init(pPort, u8Pin, &tdPortPinCfg))
                {
                    u8HalResult = DRIVER_TRUE;
                }
                else
                {
                    u8HalResult = DRIVER_PARAMETER_ERROR;
                }
            }
            else
            {
                u8HalResult = DRIVER_PARAMETER_ERROR;
            }
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }

    UNUSED(tdPortPinCfg);
    return u8HalResult;
}

uint8_t HAL_GPIO_Pin_FastInit(uint8_t u8PinNum, gpio_pin_config_t *tsConfig)
{
    uint8_t u8HalResult;
    
    if(NULL != tsConfig)
    {
        
        
        if(((tsConfig->u8DriveMode == CY_GPIO_DM_ANALOG)                ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLUP_IN_OFF)         ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLDOWN_IN_OFF)       ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_OD_DRIVESLOW_IN_OFF)   ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_OD_DRIVESHIGH_IN_OFF)  ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_STRONG_IN_OFF)         ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLUP_DOWN_IN_OFF)    ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_HIGHZ)                 ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLUP)                ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLDOWN)              ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_OD_DRIVESLOW)          ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_OD_DRIVESHIGH)         ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_STRONG)                ||\
            (tsConfig->u8DriveMode == CY_GPIO_DM_PULLUP_DOWN) )         &&\
            (tsConfig->u8OutValue == PIN_LOW) || (tsConfig->u8OutValue == PIN_HIGH) 
           )
            
        {
            
            MAPPING_PORT_PIN(u8PinNum);
            
            if ((stc_GPIO_PRT_t *)INVALID_GPIO != pPort)
            {
                Cy_GPIO_Pin_FastInit(pPort,u8Pin, 
                                     tsConfig->u8DriveMode,
                                     tsConfig->u8OutValue,
                                     (en_hsiom_sel_t)tsConfig->u8HSIOM);
                
                u8HalResult = DRIVER_TRUE;
            }
            else
            {
                u8HalResult = DRIVER_PARAMETER_ERROR;
            }
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
        
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    
    return u8HalResult;
}

uint8_t HAL_GPIO_Pin_DeInit(uint8_t u8PinNum)
{
    uint8_t u8HalResult;
    cy_stc_gpio_pin_config_t tdPortPinCfg;

    MAPPING_PORT_PIN(u8PinNum);

    if ((stc_GPIO_PRT_t *)INVALID_GPIO != pPort)
    {
        memset(&tdPortPinCfg,0,sizeof(tdPortPinCfg));
        tdPortPinCfg.driveMode = CY_GPIO_DM_ANALOG;
        tdPortPinCfg.hsiom     = HSIOM_SEL_GPIO;

        if (CY_GPIO_SUCCESS == Cy_GPIO_Pin_Init(pPort, u8Pin, &tdPortPinCfg))
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_REGISTER_ERROR;
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    
    UNUSED(tdPortPinCfg);
    return u8HalResult;
}

/* This function reads the present state at the selected input pin.  */
uint8_t HAL_GPIO_PinRead(uint8_t u8PinNum, uint8_t* pReturn)
{
    uint8_t u8HalResult;
    
    MAPPING_PORT_PIN(u8PinNum);

    if ((stc_GPIO_PRT_t*)INVALID_GPIO != pPort && NULL != pReturn)
    {
        *pReturn = Cy_GPIO_Read(pPort,u8Pin);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    
    return u8HalResult;
}

/* This function writes/drives the "value" on the selected I/O line/pin. */
uint8_t HAL_GPIO_PinWrite(uint8_t u8PinNum, uint8_t u8PinValue)
{
    uint8_t u8HalResult;
    
    MAPPING_PORT_PIN(u8PinNum);
    if (((stc_GPIO_PRT_t*)INVALID_GPIO != pPort) && (PIN_LOW == u8PinValue || PIN_HIGH == u8PinValue))
    {
        Cy_GPIO_Write(pPort,u8Pin, (uint32_t) u8PinValue);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR; 
    }

    return u8HalResult;
}

/* This function toggles/inverts the present value on the selected I/O line/pin. */
uint8_t HAL_GPIO_PinToggle(uint8_t u8PinNum)
{
    uint8_t u8HalResult;

    MAPPING_PORT_PIN(u8PinNum);
    if ((stc_GPIO_PRT_t*)INVALID_GPIO != pPort)
    {
        Cy_GPIO_Inv(pPort,u8Pin);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR; 
    }

    return u8HalResult;
}

#if GPIO_INTERRUPT_TEST

#define CYHAL_GPIO_IRQn   CPUIntIdx7_IRQn
#define NUM_OF_GPIO_CALLBACK  (8u)

typedef struct 
{
    volatile stc_GPIO_PRT_t* pPort;
    uint8_t u8Pin;
    HAL_GPIO_ISR pISR;
}hal_gpio_isr_t;

static hal_gpio_isr_t tdGpio_Callback[NUM_OF_GPIO_CALLBACK] = {0u};
static uint8_t u8NumOfCallback = 0u;

static void HAL_GPIO_IRQ(void)
{
    for (uint8_t i = 0 ; i < u8NumOfCallback ; i++)
    {
        /* If gpio falling / rising edge detected */
        uint32_t u32IntStatus = Cy_GPIO_GetInterruptStatusMasked(tdGpio_Callback[i].pPort,tdGpio_Callback[i].u8Pin);
        if (u32IntStatus != 0uL)
        {
            (*tdGpio_Callback[i].pISR)();
            Cy_GPIO_ClearInterrupt(tdGpio_Callback[i].pPort, tdGpio_Callback[i].u8Pin);
        }
    }
}

static uint8_t HAL_IRQ_Config_Check(volatile stc_GPIO_PRT_t* pPort,uint32_t u32IntEdge, HAL_GPIO_ISR pCallback)
{
    uint8_t u8HalResult;
    if ((NULL != pPort) && (NULL != pCallback))
    {
     
        if ( ((CY_GPIO_INTR_RISING  == u32IntEdge)  ||\
              (CY_GPIO_INTR_FALLING == u32IntEdge)  ||\
              (CY_GPIO_INTR_BOTH    == u32IntEdge)) &&\
              (u8NumOfCallback < NUM_OF_GPIO_CALLBACK)
            )
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
                     
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_GPIO_Callback_Register(uint8_t u8PinNum, uint32_t u32IntEdge , HAL_GPIO_ISR pCallback)
{
    uint8_t u8HalResult;
    MAPPING_PORT_PIN(u8PinNum);
    
    if (DRIVER_TRUE == HAL_IRQ_Config_Check(pPort,u32IntEdge,pCallback))
    {
        /** Set GPIO callback context*/
        tdGpio_Callback[u8NumOfCallback].pPort  = pPort;
        tdGpio_Callback[u8NumOfCallback].u8Pin  = u8Pin;
        tdGpio_Callback[u8NumOfCallback].pISR   = pCallback;
        u8NumOfCallback++;
        
        Cy_GPIO_SetInterruptEdge(pPort, u8Pin, u32IntEdge);
        Cy_GPIO_ClearInterrupt  (pPort, u8Pin);
        Cy_GPIO_SetInterruptMask(pPort, u8Pin, 1u);
        
        /* Setup GPIO interrupt */
        cy_stc_sysint_irq_t tdIRQ_Cfg =
        {
            .sysIntSrc  = (cy_en_intr_t)(ioss_interrupts_gpio_0_IRQn + u8PortNum),
            .intIdx     = CYHAL_GPIO_IRQn,
            .isEnabled  = true,
        };
        Cy_SysInt_InitIRQ(&tdIRQ_Cfg);
        Cy_SysInt_SetSystemIrqVector(tdIRQ_Cfg.sysIntSrc, HAL_GPIO_IRQ);
        /* Set the Interrupt Priority & Enable the Interrupt */
        NVIC_SetPriority(CYHAL_GPIO_IRQn, CYHAL_SCB_IRQ_PRIORITY);
        NVIC_ClearPendingIRQ(CYHAL_GPIO_IRQn);
        NVIC_EnableIRQ(CYHAL_GPIO_IRQn);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

#endif
/*** End of file ***/
