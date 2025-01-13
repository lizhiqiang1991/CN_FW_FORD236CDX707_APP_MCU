/***************************************************************************
*\file hal_pwm.h 
****************************************************************************/

#ifndef HAL_PWM_H
#define HAL_PWM_H

uint8_t HAL_PWM_Init       (uint8_t u8PwmGroup, pwm_config_t* tsConfig);
uint8_t HAL_PWM_DeInit     (uint8_t u8PwmGroup);
uint8_t HAL_PWM_Active     (uint8_t u8PwmGroup, uint8_t u8Enable);
uint8_t HAL_PWM_Start      (uint8_t u8PwmGroup, uint8_t u8Enable);

uint8_t HAL_PWM_Period_Set (uint8_t u8PwmGroup, uint16_t u16Period);
uint8_t HAL_PWM_Period_Get (uint8_t u8PwmGroup, uint16_t* pReturn);
uint8_t HAL_PWM_Duty_Get   (uint8_t u8PwmGroup, uint16_t* pReturn);
uint8_t HAL_PWM_Duty_Set   (uint8_t u8PwmGroup, uint16_t u16Duty);

uint8_t HAL_PWM_Output_Invert (uint8_t u8PwmGroup, uint8_t u8Enable);
uint8_t HAL_PWM_Interrupt_Set (uint8_t u8PwmGroup, uint8_t u8Enable, uint8_t u8Priority);
uint8_t HAL_PWM_Callback_Register (uint8_t u8PwmGroup, PWM_ISR pPwmCallback, void* pContext);

#endif

  