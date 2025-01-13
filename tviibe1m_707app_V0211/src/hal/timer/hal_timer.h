/***************************************************************************
*\file hal_timer.h 
****************************************************************************/

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

uint8_t HAL_Timer_Init              (uint8_t u8TimerGroup, timer_config_t* tsConfig); 
uint8_t HAL_Timer_DeInit            (uint8_t u8TimerGroup);
uint8_t HAL_Timer_Active            (uint8_t u8TimerGroup, uint8_t u8Enable);
uint8_t HAL_Timer_Start             (uint8_t u8TimerGroup, uint8_t u8Enable);
uint8_t HAL_Timer_Counter_Clear     (uint8_t u8TimerGroup);
uint8_t HAL_Timer_Read              (uint8_t u8TimerGroup, uint32_t * pReturn);
uint8_t HAL_Timer_Interrupt_Set     (uint8_t u8TimerGroup, uint8_t u8Enable, uint8_t u8Priority);
uint8_t HAL_Timer_Flag_Clear        (uint8_t u8TimerGroup);
uint8_t HAL_Timer_Flag_Get          (uint8_t u8TimerGroup);
uint8_t HAL_Timer_Callback_Register (uint8_t u8TimerGroup, Timer_ISR pTimerCallback, void* pContext);

#endif

  