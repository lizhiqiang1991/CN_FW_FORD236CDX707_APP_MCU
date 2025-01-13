/*

File    : testbed_pwm_duty_interrupt.c
Purpose : PWM testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the pwm output pin*/
#define PWM_PIN_LINE_38     KIT_LED_4_BLUE  /*PWM OUTPUT TO LED IO_P12_2*/

#define USER_PWM_PIN_LINE   PWM_PIN_LINE_38
#define USER_PWM_GROUP      PWM_GROUP0_CNT38

#define USER_LED            KIT_LED_1_BLUE
#define USER_BUTTON         KIT_BUTTON_PIN  /*IO_P7_0*/

/*******************************************************************/
/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     IO_P0_1 /*EVB on board USB to UART*/  
#define RX_PIN_UART     IO_P0_0                              
#define UART_SCB        UART_GROPU0                          

#ifdef NDEBUG
#define DEBUG_PF(...)
#else
#define DEBUG_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)
#endif

#define INIT_PWM_PERIOD (10000u)
#define INIT_PWM_DUTY (0u)

/*PWM Configuration*/
static pwm_config_t ts_1K_hz_Config = 
{
  /*
  Peri clock = 80Mhz
  u8PreScaler = 8 , 80Mhz/8 = 10Mhz
  Period = 10,000 , 10Mhz / 10,000 = 1Khz 
  Compare = 100  , 1% PWM duty (100/10,000)
  PWM OUTPUT => 1Khz , 1% duty 
  */          
  .u8PreScaler        = PWM_PRESCALER_DIVBY_8,  /**< 1/2/4/8/16/32/64/128 */
  .u8Pin              = USER_PWM_PIN_LINE,      /**< pwm output pin*/
  .u8PinComplementary = IO_NC,                  /**< An optional, additional inverted output pin.*/
  .u16Period          = INIT_PWM_PERIOD,        /**< 0 ~ 65535*/
  .u16Duty            = INIT_PWM_DUTY,          /**< 0 ~ 65535 Compare */
  .u8Mode             = PWM_LEFT_ALIGN,         /**< PWM alignment */
  .u8RunType          = PWM_CONTINUOUS,         /**< Continuous or One Shot*/ 
  .u8Invert           = FUNC_DISABLE,           /**< An option for the user to invert the PWM output*/
  .u8InterruptEnable  = FUNC_DISABLE,           /**< Enable/Disable Interrupt*/
  .u8NvicPriority     = 0U,                     /**< ISR priority*/
  .u32Timeout         = 0U,                     /**< PWM Config timeout */
};

static void on_board_uart_init(void)
{
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    DEBUG_PF ("\033[1J" "\033[0;0H");
}

static volatile  uint16_t u16Count = INIT_PWM_DUTY;

static void PWM_ISR_Handler(void* context)
{
    UNUSED (context);
    /* Toggle LED after 1000 PWM pulses */
    if (u16Count >= ts_1K_hz_Config.u16Period)
    {
        u16Count = 0;
        HAL_GPIO_PinToggle(USER_LED);
        
    }
    else
    {
        u16Count ++;
    }
    HAL_PWM_Duty_Set(USER_PWM_GROUP, u16Count);
}

void testbed_pwm_duty_interrput(void) 
{
    __enable_irq();
    on_board_uart_init();
   
    DEBUG_PF("testbed_pwm_duty_interrput\n");
    
    /*LED GPIO config as output*/
    HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
  
    /*Init , active and start PWM*/ 
    HAL_PWM_Init  (USER_PWM_GROUP, &ts_1K_hz_Config);
    HAL_PWM_Active(USER_PWM_GROUP, FUNC_ENABLE);
    HAL_PWM_Callback_Register(USER_PWM_GROUP,PWM_ISR_Handler,NULL);
    HAL_PWM_Start (USER_PWM_GROUP, FUNC_ENABLE);
  
    
    for (;;)
    {  
        Cy_SysLib_Delay(10);  /* Delay 10 msec */
    }
}

