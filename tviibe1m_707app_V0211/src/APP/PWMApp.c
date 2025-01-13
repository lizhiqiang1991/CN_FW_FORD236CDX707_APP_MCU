/**
* @file PWMApp.c
* @author your name (you@domain.com)
* @brief 
* @version 0.1
* @date 2021-02-02
* 
* @copyright Copyright (c) 2021
* 
*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "PWMApp.h"
#include "GpioApp.h"
#include "Mod_BLBrightness_Ctrl.h"
#include "UartApp.h"
#include "gPinDef.h"
#include "Mod_DataManagement.h"

#define DUTY_NUM_ZERO 0x0U


#define INIT_PWM_DUTY (0u)
#define PWM_DUTY_50_PERCENT (500u)

static uint16_t BL_PWMDuty_t = DUTY_NUM_ZERO;
static uint16_t COOL_PWMDuty_t = DUTY_NUM_ZERO;
static volatile uint16_t BL_PWMDuty = DUTY_NUM_ZERO;
static volatile uint16_t COOL_PWMDuty = DUTY_NUM_ZERO;
static uint16_t u16PWMper_getreg = DUTY_NUM_ZERO;
//static uint16_t u16PWMDuty_getregISR = DUTY_NUM_ZERO;
//static uint16_t u16PWMDuty_getregISR2= DUTY_NUM_ZERO;

uint16_t BL_Pwm_Duty_Output_Get(void);
uint16_t COOLING_FAN_Pwm_Duty_Output_Get(void);




/*PWM Configuration*/
static pwm_config_t ts_20K_hz_Config = 
{
    /*
    Peri clock = 80Mhz
    u8PreScaler = 2 , 80Mhz/4 = 20Mhz
    Period = 1,000 , 20Mhz / 1,000 = 20Khz 
    Compare = 10  , 1% PWM duty (100/1,000)
    PWM OUTPUT => 1Khz , 1% duty 
    */          
    .u8PreScaler        = PWM_PRESCALER_DIVBY_4,  /**< 1/2/4/8/16/32/64/128 */
    .u8Pin              = PIN_COOLING_FAN_PWM,      /**< pwm output pin*/
    .u8PinComplementary = IO_NC,                  /**< An optional, additional inverted output pin.*/
    .u16Period          = PERIOD_20kHz - 1U,           /**< 0 ~ 1000 */
    .u16Duty            = INIT_PWM_DUTY,          /**< 0 ~ 1000 Compare */
    .u8Mode             = PWM_LEFT_ALIGN,         /**< PWM alignment */
    .u8RunType          = PWM_CONTINUOUS,         /**< Continuous or One Shot*/ 
    .u8Invert           = FUNC_DISABLE,           /**< An option for the user to invert the PWM output*/
    .u8InterruptEnable  = FUNC_DISABLE,           /**< Enable/Disable Interrupt*/
    .u8NvicPriority     = 1U,                     /**< ISR priority*/
    .u32Timeout         = 0U,                     /**< PWM Config timeout */
};

//static pwm_config_t gts_610_hz_35_Config = 
//{
//    /*
//    Peri clock = 80Mhz
//    u8PreScaler = 4 , 80Mhz/4 = 20Mhz
//    Period = 32,768 , 20Mhz / 32,768 = 610.35hz 
//    Compare = 16,384  , 50% PWM duty (16,384/32,768 )
//    PWM OUTPUT => 2Khz , 50% duty 
//    */          
//    .u8PreScaler        = PWM_PRESCALER_DIVBY_4,  /**< 1/2/4/8/16/32/64/128 */
//    .u8Pin              = PIN_TCON_PWMI,                  /**< pwm output pin*/
//    .u8PinComplementary = IO_NC,                  /**< An optional, additional inverted output pin.*/
//    .u16Period          = PERIOD_650HZ35,                 /**< 0 ~ 65535*/
//    .u16Duty            = INIT_PWM_DUTY,                  /**< 0 ~ 65535 Compare */
//    .u8Mode             = PWM_LEFT_ALIGN,         /**< PWM alignment */
//    .u8RunType          = PWM_CONTINUOUS,         /**< Continuous or One Shot*/ 
//    .u8Invert           = FUNC_DISABLE,            /**< An option for the user to invert the PWM output*/
//    .u8InterruptEnable  = FUNC_DISABLE,            /**< Enable/Disable Interrupt*/
//    .u8NvicPriority     = 1U,                     /**< ISR priority*/
//    .u32Timeout         = 0U,                     /**< PWM Config timeout */
//};

//static void TCON_PWM_ISR_Handler(void* context)
//{
//    UNUSED (context);
//    
//    BL_PWMDuty =  BL_Pwm_Duty_Output_Get();
//    HAL_PWM_Duty_Set(TCON_PWM_GROUP, BL_PWMDuty);
//    HAL_PWM_Duty_Get(TCON_PWM_GROUP, &u16PWMDuty_getregISR);
//}
//
//static void COOLING_FAN_PWM_ISR_Handler(void* context)
//{
//    UNUSED (context);
//    
//    COOL_PWMDuty = COOLING_FAN_Pwm_Duty_Output_Get();
//    HAL_PWM_Duty_Set(COOLING_FAN_PWM_GROUP, COOL_PWMDuty);
//    HAL_PWM_Duty_Get(COOLING_FAN_PWM_GROUP, &u16PWMDuty_getregISR2);
//}


void PWM_Initialze (void)
{
    __enable_irq();
    /*
    HAL_PWM_Init  (TCON_PWM_GROUP, &gts_610_hz_35_Config);
    HAL_PWM_Active(TCON_PWM_GROUP, FUNC_ENABLE);
    HAL_PWM_Callback_Register(TCON_PWM_GROUP,TCON_PWM_ISR_Handler,NULL);
    
    BL_PWMDuty = DUTY_NUM_ZERO;
    BL_PWMDuty_t = DUTY_NUM_ZERO;
    */
    COOL_PWMDuty = DUTY_NUM_ZERO;
    COOL_PWMDuty_t = PWM_DUTY_50_PERCENT;

    
    /*Init , active and start PWM*/ 
    HAL_PWM_Init  (COOLING_FAN_PWM_GROUP, &ts_20K_hz_Config);
    HAL_PWM_Active(COOLING_FAN_PWM_GROUP, FUNC_ENABLE);
    //HAL_PWM_Callback_Register(COOLING_FAN_PWM_GROUP,COOLING_FAN_PWM_ISR_Handler,NULL);
    //HAL_PWM_Duty_Set(COOLING_FAN_PWM_GROUP, COOL_PWMDuty_t);
    HAL_PWM_Start (COOLING_FAN_PWM_GROUP, FUNC_ENABLE);
    //HAL_PWM_Start (TCON_PWM_GROUP, FUNC_ENABLE);
    //COOLING_FAN_Pwm_Duty_Output(COOLING_FAN_PWM_GROUP, 500);
    
}



void Pwm_Duty_Output_Get(uint8_t u8PwmGroup, uint16_t *u16Duty)									// Duty = 0 ~ 100 (0% ~ 100%), Unit is 1%
{
    (void)HAL_PWM_Duty_Get( u8PwmGroup, u16Duty);
}

uint16_t BL_Pwm_Duty_Output_Get(void)
{
    return BL_PWMDuty_t;
}

void BL_Pwm_Duty_Output(uint8_t u8PwmGroup, uint16_t u16Duty)									// Duty = 0 ~ 100 (0% ~ 100%), Unit is 1%
{  
    static uint8_t pwm_OV_flag = 0U; 
    if(u16Duty>=PERIOD_650HZ35)
    {
        u16Duty = MAX_TCON_DUTY;
        if(pwm_OV_flag == false)
        {
            HAL_GPIO_PinWrite(PIN_TCON_PWMI,PIN_HIGH);
            HAL_GPIO_Pin_Init(PIN_TCON_PWMI,&gtdPinOutputConfig_PWM_GPIO);       
            HAL_GPIO_PinWrite(PIN_TCON_PWMI,PIN_HIGH);
        }
        pwm_OV_flag =  true;
    }
    
    if(u16Duty <  PERIOD_650HZ35)
    {
        if(pwm_OV_flag == true)
        {
            HAL_GPIO_Pin_Init(PIN_TCON_PWMI,&tsGpioCfg_GPIO_PWM);   
            HAL_PWM_Duty_Set(u8PwmGroup, u16Duty);          
        }
        pwm_OV_flag = false; 
    }
    
    if(u16PWMflage==1 )
    {
        BL_PWMDuty_t = u16Duty;
    }
    else
    {
        BL_PWMDuty_t = u16Duty; 
    }
    
    HAL_PWM_Period_Get (u8PwmGroup, &u16PWMper_getreg);
}


uint16_t COOLING_FAN_Pwm_Duty_Output_Get(void)
{
    return COOL_PWMDuty_t;
}

void COOLING_FAN_Pwm_Duty_Output_Factory_Mode(uint8_t u8PwmGroup1, uint16_t u16Duty1)									// Duty = 0 ~ 100 (0% ~ 100%), Unit is 1%
{ 
     HAL_PWM_Duty_Set(u8PwmGroup1, u16Duty1);  
}
void COOLING_FAN_Pwm_Duty_Output(uint8_t u8PwmGroup, uint16_t u16Duty)									// Duty = 0 ~ 100 (0% ~ 100%), Unit is 1%
{ 
    if (DISABLE == MMIM_FactoryFlag_Ctrl(GETFLAG))
    {
      HAL_PWM_Duty_Set(u8PwmGroup, u16Duty);
    }
    /*
    DEBUG_PF("COOLING_FAN_Pwm_Duty_Output\r\n");
    static uint8_t pwm_OV_flag = 0U; 
    if(u16Duty>=PERIOD_20kHz)
    {
        u16Duty = MAX_COOLING_FAN_DUTY;
        if(pwm_OV_flag == false)
        {
            HAL_GPIO_PinWrite(PIN_COOLING_FAN_PWM,PIN_HIGH);
            HAL_GPIO_Pin_Init(PIN_COOLING_FAN_PWM,&gtdPinOutputConfig_PWM_GPIO);       
            HAL_GPIO_PinWrite(PIN_COOLING_FAN_PWM,PIN_HIGH);
        }
        pwm_OV_flag =  true;
    }
    
    if(u16Duty <  PERIOD_650HZ35)
    {
        if(pwm_OV_flag == true)
        {
            HAL_GPIO_Pin_Init(PIN_COOLING_FAN_PWM,&tsGpioCfg_GPIO_PWM);   
            HAL_PWM_Duty_Set(u8PwmGroup, u16Duty);          
        }
        pwm_OV_flag = false; 
    }
    
    COOL_PWMDuty_t = u16Duty;
    //Pwm_Duty_Output_Get(u8PwmGroup, &u16PWMDuty_getreg);
    HAL_PWM_Period_Get (u8PwmGroup, &u16PWMper_getreg);
    */
  
}



