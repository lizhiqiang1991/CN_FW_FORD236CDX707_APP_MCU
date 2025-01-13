#ifndef HAL_GPIO_H
#define HAL_GPIO_H
#include "cyip_gpio_v2.h"

/*Port operation*/
uint8_t HAL_GPIO_Port_Init   (uint32_t u32PortGroup, gpio_port_config_t *tsConfig);
uint8_t HAL_GPIO_Port_DeInit (uint32_t u32PortGroup);
uint8_t HAL_GPIO_PortRead    (uint32_t u32PortGroup,uint32_t u32Mask, uint32_t * pReturn);
uint8_t HAL_GPIO_PortWrite   (uint32_t u32PortGroup,uint32_t u32Mask, uint32_t u32PortValue);
uint8_t HAL_GPIO_PortToggle  (uint32_t u32PortGroup,uint32_t u32Mask);

/*Pin operation*/
uint8_t HAL_GPIO_Pin_Init    (uint8_t u8PinNum, gpio_pin_config_t *tsConfig);
uint8_t HAL_GPIO_Pin_FastInit(uint8_t u8PinNum, gpio_pin_config_t *tsConfig);
uint8_t HAL_GPIO_Pin_DeInit  (uint8_t u8PinNum);
uint8_t HAL_GPIO_PinRead     (uint8_t u8PinNum, uint8_t* pReturn);
uint8_t HAL_GPIO_PinWrite    (uint8_t u8PinNum, uint8_t u8PinValue);
uint8_t HAL_GPIO_PinToggle   (uint8_t u8PinNum);

static stc_GPIO_PRT_t* HAL_GPIO_PortToAddr(uint32_t u32PortNum);
static uint8_t HAL_GPIO_PinConfig_Check( gpio_pin_config_t *tsConfig);

#define GPIO_INTERRUPT_TEST 1

#if GPIO_INTERRUPT_TEST
typedef void (*HAL_GPIO_ISR)(void);
uint8_t HAL_GPIO_Callback_Register(uint8_t u8PinNum, uint32_t u32IntEdge , HAL_GPIO_ISR pCallback);
#endif

#endif

