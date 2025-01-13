#ifndef HAL_CLOCK_H
#define HAL_CLOCK_H

uint8_t HAL_Clock_Init                        (clock_config_t *tsConfig);
uint8_t HAL_Clock_PLL_Configuration           (pll_fll_config_t *tsPllConfig);
uint8_t HAL_Clock_FLL_Configuration           (pll_fll_config_t *tsFllConfig);
uint8_t HAL_Clock_High_External_Clock_Active  (uint8_t u8Enable, uint32_t u32Timeout);
uint8_t HAL_Clock_Low_External_Clock_Active   (uint8_t u8Enable, uint32_t u32Timeout);
uint8_t HAL_Clock_High_Internal_Clock_Active  (uint8_t u8Enable, uint32_t u32Timeout);
uint8_t HAL_Clock_Low_Internal_Clock_0_Active (uint8_t u8Enable, uint32_t u32Timeout);
uint8_t HAL_Clock_Low_Internal_Clock_1_Active (uint8_t u8Enable, uint32_t u32Timeout);
uint8_t HAL_Clock_PLL_Clock_Active            (uint8_t u8Enable, uint32_t u32Timeout);
uint8_t HAL_Clock_FLL_Clock_Active            (uint8_t u8Enable, uint32_t u32Timeout);
uint8_t HAL_Clock_Path_Setting                (uint8_t u8ClockSource, uint8_t u8ClockTarget);

#endif


