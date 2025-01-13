/**
* @file Adc0App.h
* @author Derek Wen (you@domain.com)
* @brief 
* @version 0.1
* @date 2020-11-27
* 
* @copyright Copyright (c) 2020
* 
*/

#ifndef ADCAPP_H_
#define ADCAPP_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "gPinDef.h"
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_config.h"
#include "hal_adc.h"
#include "hal_adc_def.h"

#define ADC_12BIT_ADC_MAX  4095U


#define ADC_MCU_VOL 33U /* 3.3v for fixed point */

#define ADC_12BIT_ADC_1V_IDEAL  (ADC_12BIT_ADC_MAX/ADC_MCU_VOL)
#define ADC_FIXEDPOINT_MODIFY_PARAMETER 10U

void AdcApp_2msTimerProcess_Install(void);

uint16_t AdcApp_BatteryVolAdcValue_Get (void);

uint16_t AdcApp_984TEMPAdcValue_Get (void);

uint16_t AdcApp_BacklightNtcAdcVaules_L_Get (void);

uint16_t AdcApp_BacklightNtcAdcVaules_R_Get (void);

uint16_t AdcApp_Ref_1V_Get (void);


void Adc_Initialize(void); 

#endif
