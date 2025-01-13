#ifndef Mod_BatteryProtection_Ctrl_H
#define Mod_BatteryProtection_Ctrl_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
    BAT_initial_mode ,
    BAT_normal_mode  ,
    BAT_highvoltage_mode ,	
    BAT_lowvoltage_mode ,
}Battery_process_TypeDef;

typedef void (*MBP_CPMEVENT_SEND)(void);
/*Units*/
uint8_t PinIO_DebounceResult(uint8_t PinIOStatus,uint16_t High2Low_cnt,uint16_t Low2High_cnt);

uint16_t MBPC_BatteryVoltAdcValues_Average(uint16_t u16BatteryVoltAdcValues);
Battery_process_TypeDef MBPC_BatteryProtection_Process(uint16_t Battery_Voltage,uint16_t u16Voltage_ActiveLow,uint16_t u16Voltage_ActiveHigh,uint8_t pin61460Falt_status);
// uint8_t MBPC_PMICRecover_Detect(uint8_t u8ActiveFlag);
uint8_t MBPC_PMICRecover_Detect(uint8_t u8ActiveFlag, uint16_t u16BatteryVoltAdcValues, uint16_t u16MCU1V_ADCValues);

/*Modeule*/
Battery_process_TypeDef MBPC_BatteryProtection_Ctrl(uint16_t u16BatteryVoltAdcValues, uint16_t u16MCU1V_ADCValues);
uint8_t MBPC_BatteryProtection_ShutdownStatus_Set(uint8_t u8Stauts);
uint8_t MBPC_BatteryProtection_Function_Initial(MBP_CPMEVENT_SEND pShutdownEventFun);

uint16_t MBPC_BattPro_BattVolt_Get(void);


#endif /* Mod_BatteryProtection_Ctrl_H */

