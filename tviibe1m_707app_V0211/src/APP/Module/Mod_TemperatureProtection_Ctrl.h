#ifndef Mod_TemperatureProtection_Ctrl_H
#define Mod_TemperatureProtection_Ctrl_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MCU_BandGap 33 /* 3.3v for fixed point */
#define FIXEDPOINT_MODIFY_PARAMETER 10
#define ADC_Value_Max 4095U
// #define SIZE_PCBNTC 166U
#define initial_mode   1U
#define normal_mode    2U
#define derating_mode  3U
#define boot_mode      4U
#define shutdown_mode  5U

#define PWM_Resolution_10bit 1023U
#define temperature_diff_record 5U /* 20220220 Joel */
#define BootMode_delaytimes 10U
#define ShutDownrecover_delaytimes 10U
#define Reset_Counter 0U

#define TEMPSOUREFROM_BL_R 1U
#define TEMPSOUREFROM_BL_L 2U
#define TEMPSOUREFROM_984  3U

#define Reset_Index 0U
#define u8PCBTempBuffer_size 32U    /* from 8 to 32 */

#define Negative 1U
#define Positive 0U

#define min(x,y)         (x < y?x:y)
#define max(x,y)         (x > y?x:y)


typedef struct
{
    uint8_t DataSource;
    uint8_t symbol;
    uint16_t Temperature_Value;
}Mod_TemPro_Ctrl_TableTypedef;



/*Units*/
uint16_t MTPC_BacklightNtcAdcValue_Average_API_L(uint16_t u16BacklightNtcAdcValue);
uint16_t MTPC_BacklightNtcAdcValue_Average_API_R(uint16_t u16BacklightNtcAdcValue);
uint16_t MTPC_PCBNtcAdcValue_Average_API(uint16_t u16BacklightNtcAdcValue);

int16_t MTPC_BacklightNtcAdcValue_lookuptable_API(uint16_t In1,uint16_t Table_x[],int16_t Table_y[],uint8_t maxIndex);
uint32_t MTPC_Backlight_derating_process(int16_t i16Temp,uint16_t u16TEMP_NTC_0,uint16_t u16TEMP_NTC_74,uint16_t u16TEMP_NTC_82,uint16_t u16Duty_full,uint16_t u16Duty_end, uint16_t u16BacklightUserDuty_Get);
/*Modeule*/
uint32_t MTPC_TemperatureProtection_Ctrl(uint8_t DataSource, uint16_t u16BacklightNtcAdcValue,uint16_t u16MCURef_1V_ADCValues,uint16_t u16BacklightUserDuty_Get);


Mod_TemPro_Ctrl_TableTypedef Mod_TemPro_Ctrl_Table_Get(void);
int16_t MTPC_Temperature_Get(uint8_t DataSource, uint16_t u16BacklightNtcAdcValue,uint16_t u16MCURef_1V_ADCValues);

#endif /* Mod_TemperatureProtection_Ctrl_H */
