/*
* Mod_BLbrightness_Ctrl.h
*
*  Created on: 2020�~11��10��
*      Author: DerekWen
*/

#ifndef MOD_BLBRIGHTNESS_CTRL_H_
#define MOD_BLBRIGHTNESS_CTRL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define PWMValMergeShift    8U
#define BL_PWM_Channel      PWM_GROUP0_CNT37
#define BLBC_DATABYTE_ONE    0U
#define BLBC_DATABYTE_TWO    1U
#define BLBC_DATABYTE_THR    2U
#define BLBC_DATABYTE_FOR    3U
#define TCON_MAX_DUTY    65535U
#define MAX_10BIT_PWM_SET_VAlUE 1023U
#define FIXEDPOINT_10       10U

typedef enum
{
    eProcess_Ready = 0U,
    eDimmingCal_finished,
    eNonDimmingCtrl_finished
}MBLBC_BLBrightness_status_E;

typedef enum
{
    eIncrease = 0U,
    eDecrease
}PWMOperator_E;

typedef struct
{
    uint8_t PWMpreStep;
    PWMOperator_E PWMOperator;
    uint8_t ucDimmingStep;
    uint16_t u16TargetBLPWM_10bit;
    uint8_t DimmingCalculateFinish;
    uint8_t DimmingActionFlag;
}PWMDimmingStep_T;

extern uint16_t u16PWMflage;
MBLBC_BLBrightness_status_E MBLBC_BLBrightness_Ctrl(uint8_t ucBLPWMVal_8_9,uint8_t ucBLPWMVal_0_7,uint8_t ucDimmingEnable);
void MBLC_IRQCallBack_Install (void);
uint8_t MBLC_TconApp_Backlight_Output(uint16_t u16DutyValue);
uint8_t Get_Dimming_Action_flag(void);
uint16_t MBLBC_BLPWM_Output(uint16_t u16TargetBLPWM_10bit);
#endif /* MOD_BLBRIGHTNESS_CTRL_H_ */
