/***************************************************************************//**
* \file main_cm0plus.c
*
* \brief
* Main file for CM0+
*
********************************************************************************
* \copyright
* Copyright 2016-2019, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>

/* include public information */
#include "gPinDef.h"
#include "Ford_Config.h"
/* include component */
#include "Com_PowerManagement.h"
#include "Com_Communication.h"
#include "Com_DeviceControl.h"
#include "Com_Diagnostic.h"
#include "Com_BatteryProtect.h"
#include "Com_TemperatureProtect.h"
/* include module */
#include "Mod_BatteryProtection_Ctrl.h"
#include "Mod_DataManagement.h"
#include "Mod_PowerSequence_Ctrl.h"
#include "Mod_FPNCtrl.h"

/* Include DriverApp */
#include "ClockApp.h"
#include "TimerApp.h"
#include "GpioApp.h"
#include "I2CSlaveApp.h"
#include "I2CMasterApp.h"
#include "PWMApp.h"
#include "UartApp.h"
#include "AdcApp.h"
#include "FlashApp.h"
#include "WDTApp.h"

/* Include Backdoor */
#include "ICDiagApp.h"

#define DebugMsg 0U


/* Todo: add below flag to config file
         1. Open Functional Safefy
         2. Open Autorun mode
         3. Open Backdoor diag
*/

int main(void)
{
//     static const char* pString = "\n\nUART Polling testbed\n";   
//    uint16_t u16counter = 0;
    
    #ifndef CY_SEMIHOSTING_DISABLED
    

    

    #if (UART_POLLING_TESTBED == DebugMsg)
       /*Don't use uart semihosting */
    #else
      /*redirect printf to UART (SCB3) , P13.0 (RX) / P13.1 (TX)*/
      Cy_Semihosting_InitAll(CY_USB_SCB_TYPE,115200,NULL,false);
    #endif
    #endif
    /* MCU Initialize */
    __enable_irq();
    Clock_Initialize();
    PWM_Initialze();
    UartApp_initialize();
    GPIO_Initialize();
    Timer_Initialize();
    Adc_Initialize();  
    Flash_Initialize();  
#ifdef AUTORUN
    /* In autorun mode, stop watch dog function */
#else
    WDT_Initialize();
#endif
//    I2CM_Initialze();
    
#if BACKDOOR_ICDIAG_OPEN
    ICDIAG_Initialize();
#endif
#if 1
    //Cy_SysLib_Delay(100);  // Delay 100 msec
    while(1)
    {

      /* 1ms porcess */
      if(HAL_Timer_Flag_Get(TIMER_1MINISEC) == DRIVER_TRUE)
      {
//        u16counter++;
        CPM_State_Machine[sPmState]();
        CC_State_Machine[CC_StateMachine_e]();   
        CTP_State_Machine[CTP_StateMachine_e]();
        CDC_State_Machine[CDC_StateMachine_e]();
        CBP_State_Machine[CBP_StateMachine_e]();
        CD_State_Machine[CD_StateMachine_e]();
        
        /* 1ms Process End */
        
#if BACKDOOR_ICDIAG_OPEN
        //MFPNC_FPNCoreAssmToFlash_Write(pCore_assembly,128);  
        ICDIAG_Main();
#endif        
        
        (void)HAL_Timer_Flag_Clear(TIMER_1MINISEC);
      }
      /* Clears ("feeds") the watchdog, to prevent a XRES device reset. */ 
      WDTApp_ClearWDT ();
    
      //MDEN_TConBistMode_En(3); 
    }
#else  
 
        CPM_State_Machine[sPmState]();
        CC_State_Machine[CC_StateMachine_e]();        
        CDC_State_Machine[CDC_StateMachine_e]();
        CD_State_Machine[CD_StateMachine_e]();
 
    
    while(1)
    {
    
      //MBLBC_BLBrightness_Ctrl(0x03U,0xFFU,0x01U);
      //Cy_SysLib_Delay(800);
      //MBLBC_BLBrightness_Ctrl(0x00U,0x00U,0x01U);
      //Cy_SysLib_Delay(800);
      for(uint16_t i =0U; i<=32768;i=i+1)
      {
        Pwm1_Duty_Output(USER_PWM_GROUP, i);
      }
            Cy_SysLib_Delay(10);

      for(uint16_t j =32768U; j>0;j=j-1)
            {
     
        Pwm1_Duty_Output(USER_PWM_GROUP, j);
        
      }    
            Cy_SysLib_Delay(10);

    }
#endif
}
