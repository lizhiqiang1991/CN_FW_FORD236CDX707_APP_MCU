/**
* @file TimerApp.h
* @author Derek Wen
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

/* Define */
#define TIMER_1MINISEC                  TIMER_GROUP0_CNT0
#define WDT_TIMER_10MINISEC             TIMER_GROUP0_CNT1
#define MBLBC_PWMDimming_TIMER_2MINSEC  TIMER_GROUP0_CNT2
#define MINTBC_TIMER_1MINSEC            TIMER_GROUP0_CNT4
#define CD_DET_TIMER_25MINSEC           TIMER_GROUP0_CNT5
#define ADC_DET_TIMER_2MINSEC           TIMER_GROUP0_CNT6
#define PMIC_DET_TIMER_1MINSEC          TIMER_GROUP0_CNT7
#define CD_DET_TIMER_10MINSEC           TIMER_GROUP0_CNT9
#define CD_DET_TIMER_50MINSEC           TIMER_GROUP0_CNT10
#define TIMER_2MINISEC_INTB             TIMER_GROUP0_CNT11
#define TIMER_10MINISEC                 TIMER_GROUP0_CNT12
#define CD_DET_TIMER_1MINSEC            TIMER_GROUP0_CNT13

void Timer_Initialize (void);
void Timer_10msProcess_Main (void);