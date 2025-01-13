/**
* @file PWMApp.h
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-02-02
* 
* @copyright Copyright (c) 2021
* 
*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"

#define PERIOD_650HZ35       (32768U)
#define MAX_TCON_DUTY        (PERIOD_650HZ35)

#define PERIOD_20kHz             (1000U)
#define MAX_COOLING_FAN_DUTY    (PERIOD_20kHz)
#define COOLING_FAN_DUTY_50     (500U)
#define COOLING_FAN_DUTY_1PA    (10U)


#define TCON_PWM_GROUP          PWM_GROUP0_CNT37
#define COOLING_FAN_PWM_GROUP   PWM_GROUP0_CNT13

void PWM_Initialze (void);
void BL_Pwm_Duty_Output(uint8_t u8PwmGroup, uint16_t u16Duty);
void COOLING_FAN_Pwm_Duty_Output_Factory_Mode(uint8_t u8PwmGroup1, uint16_t u16Duty1);
void COOLING_FAN_Pwm_Duty_Output(uint8_t u8PwmGroup, uint16_t u16Duty);
uint16_t COOLING_FAN_Pwm_Duty_Output_Get(void);
