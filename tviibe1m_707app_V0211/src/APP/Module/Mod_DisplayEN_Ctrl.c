/******************************************************************************
;       Program  : Mod_DisplayEN_Ctrl.c
;       SWDD_ID  : SWDD_001
;       Project  : CP49ACM010
;       Chip     : Cypress CYT2B6
;       Date     : 2021 / 6 / 8
;       Author   : Joshua
;       Describe : 根據Component來的指令，10ms內完成Control display device enable or disable，
;       在subaddress 0x05 為shutdown 的狀態下，則忽略該Enable的動作(SWRA-01-055)，
;       Display device enable:I2CM write $11、$29 to TD7800
;       Display device disable:I2CM write $28、$10 to TD7800
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
/* include global */
#include "gPinDef.h"

/* include module */
#include "Mod_DisplayEN_Ctrl.h"
#include "Mod_DataManagement.h"
#include "Mod_BLBrightness_Ctrl.h"
#include "Mod_PowerSequence_Ctrl.h"
#include "Mod_BLEN_ctrl.h"
/* include APP */
#include "GpioApp.h"
#include "TimerApp.h"
#include "I2CMasterApp.h"
#include "PWMApp.h"
#include "Mod_DetectI2C.h"
#include "UartApp.h"


/*------------------------ Declare External Var -----------------------------*/
static Global_PowerState_E eDispEN_PowerState;
static uint8_t u8DispEN_PanelState;

static uint8_t uDataCDC1[4]={NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO};
static uint8_t uDataCDC2[4]={NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO};
static uint8_t uDataCDC3[4]={NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO};
static uint8_t uDataCDC4[4]={NUM_ZERO,NUM_ZERO,NUM_ZERO,NUM_ZERO};
/*---------------------------- Start Program ---------------------------------*/


/******************************************************************************
;       Function Name			:	void MDEN_SourceDriver_DispOn_CheckReset(void)
;       Function Description	:	
;       Parameters				:	None
;       Return Values			:	None
;		Source ID				:	
******************************************************************************/
void MDEN_SourceDriver_DispOn_CheckReset(void)
{    
    /* HX82105 IC 	Source diver fault address data */

    (void)I2CM_Write(0x59U, 0x60U, &uDataCDC1[1], sizeof(uDataCDC1[1]));
    (void)I2CM_Write(0x5AU, 0x60U, &uDataCDC2[1], sizeof(uDataCDC2[1]));
    (void)I2CM_Write(0x58U, 0x60U, &uDataCDC3[1], sizeof(uDataCDC3[1]));
    (void)I2CM_Write(0x5BU, 0x60U, &uDataCDC4[1], sizeof(uDataCDC4[1]));
}

/******************************************************************************
;       Function Name			:	void MDEN_SourceDriver_DispOn_CheckReset(void)
;       Function Description	:	
;       Parameters				:	None
;       Return Values			:	None
;		Source ID				:	
******************************************************************************/
void MDEN_TConBistMode_En(uint8_t TConBistMode_Ctrl)
{
    uint8_t ucDispBistEN = DISABLE;
    uint8_t ucBistPattenStart = DISABLE;
    
    ucDispBistEN = TConBistMode_Ctrl & MASK_1_BIT;
    ucBistPattenStart = TConBistMode_Ctrl & BIT2;
    
    ucBistPattenStart = ucBistPattenStart >> 1;
    
    if(ucDispBistEN == ENABLE)
    {      
        (void)HAL_GPIO_PinWrite(PIN_TCON_BIST,PIN_HIGH);     // BIST ensable
        
        if(ucBistPattenStart == ENABLE)
        {
            (void)HAL_GPIO_PinWrite(PIN_TCON_BIST_STOP,PIN_HIGH);     // pattern start
        }
        else
        {
            (void)HAL_GPIO_PinWrite(PIN_TCON_BIST_STOP,PIN_LOW);     // pattern stop
        }
    }
    else
    {
        (void)HAL_GPIO_PinWrite(PIN_TCON_BIST,PIN_LOW);     // PIN_TCON_BIST ,BIST disable
        if(ucBistPattenStart == ENABLE)
        {
            (void)HAL_GPIO_PinWrite(PIN_TCON_BIST_STOP,PIN_HIGH);     // pattern start
        }
        else
        {
            (void)HAL_GPIO_PinWrite(PIN_TCON_BIST_STOP,PIN_LOW);     // pattern stop
        }
    }    
}

/******************************************************************************
;       Function Name			:	uint8_t MSEN_Panel_On(void)
;       Function Description	:	
;       Parameters				:	None
;       Return Values			:	uint8_t ProcessState
;		Source ID				:	SWE3_001_02
******************************************************************************/
uint8_t MDEN_Panel_On(void)
{
    //uint8_t u8Input = 0U;   
    
    //Cy_SysLib_Delay(20);  // Delay 20 msec,Normal Run mode
    
    /* ----- HX82105 -----*/
    
    
    //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, NUM_ZERO);
    
    //(void)MBLC_TconApp_Backlight_Output(NUM_ZERO);

    u8DispEN_PanelState = Display_Enabled;
    
    return u8DispEN_PanelState;
}

/******************************************************************************
;       Function Name			:	uint8_t MDEN_Panel_Off(void)
;       Function Description	:	
;       Parameters				:	None
;       Return Values			:	uint8_t u8DispEN_TD7800State
;		Source ID				:	SWE3_001_03
******************************************************************************/
uint8_t MDEN_Panel_Off(void)
{
    #if 0
    (void)MBLC_TconApp_Backlight_Output(NUM_ZERO);
    /*Set black pattern*/
    (void)MDI2C_Set_TCON_BlackPattern(ENABLE);

    // (void)MBLC_TconApp_Backlight_Output(NUM_ZERO);
    //         /*Set black pattern*/
    // (void)MDI2C_Set_TCON_BlackPattern(ENABLE);


    // Cy_SysLib_Delay(100);  // Delay 100 msec

    // (void)MDI2C_Set_TCON_BlackPattern(DISABLE);   
    /* ----- HX82105 -----*/
    (void)HAL_GPIO_PinWrite(PIN_PANEL_RESET,PIN_LOW); // Panel_RESET  =0
    #endif
    // Cy_SysLib_Delay(10);  // Delay 10 msec
    
    //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, NUM_ZERO);
    
    return u8DispEN_PanelState = Display_Disable;
}

/******************************************************************************
;       Function Name			:	DispEN_DispENProcess_Ctrl
;       Function Description	:	判斷unit8 ucDisplayEn、Global_PowerState_E ePowerState、DispEN_LockState_E eLockState決定是否Enable
;       Parameters				:	unit8 ucDisplayEn、Global_PowerState_E ePowerState、DispEN_LockState_E eLockState
;       Return Values			:	uint8_t DispEN_ProcessState
;		Source ID				:	SWEDD_001_01
******************************************************************************/
uint8_t MDEN_DispENProcess_Ctrl(uint8_t ucDispEN,Global_PowerState_E ePowerState,Global_LockState_E eLockState)
{
    uint8_t DispEN_ProcessState = 0U;
    
    eDispEN_PowerState = ePowerState;
    
    switch (eDispEN_PowerState)
    {
    case ePowerState_Standby:
        
        if((ucDispEN == ENABLE) && (eLockState == eLocked))
        {
            DispEN_ProcessState = MDEN_Panel_On();/*Set Flag to turn on BL 1 for normal run*/
        }
        else
        {
            ; /* code */
        }
        break;
    case ePowerState_Normal:
        if(ucDispEN == DISABLE)
        {
            /* code */
            DispEN_ProcessState = MDEN_Panel_Off();/*Set Flag to turn on BL 2 and change PowerState to Standyby*/
        }
        else
        {         
            ;           
        }
        break;
    default:
        break;
    }
    return DispEN_ProcessState;
}


