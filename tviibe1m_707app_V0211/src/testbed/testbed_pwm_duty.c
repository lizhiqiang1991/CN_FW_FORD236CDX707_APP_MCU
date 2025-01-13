/*

File    : testbed_pwm_duty.c
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

static void on_board_uart_init(void)
{
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    DEBUG_PF ("\033[1J" "\033[0;0H");
}

void testbed_dynamic_pwm_duty(void) 
{
    __enable_irq();
    on_board_uart_init();
   
    DEBUG_PF("testbed_dynamic_pwm_duty\n");
    
    /*LED GPIO config as output*/
    HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
    
    /*Init , active and start PWM*/
    gts_2K_hz_Config.u8Pin     = USER_PWM_PIN_LINE;
    gts_2K_hz_Config.u16Period = 65535; 
    gts_2K_hz_Config.u16Duty = (gts_2K_hz_Config.u16Period >> 1) + 1;
    
    HAL_PWM_Init  (USER_PWM_GROUP, &gts_2K_hz_Config);
    HAL_PWM_Active(USER_PWM_GROUP, FUNC_ENABLE);
    HAL_PWM_Start (USER_PWM_GROUP, FUNC_ENABLE);
  
    uint16_t u16Duty =  gts_2K_hz_Config.u16Duty;
    uint32_t u32Times = 1;
    uint8_t u8GoUp = false;
    /*uint16_t u16Get;*/ 
    for (;;)
    {  
        Cy_SysLib_Delay(10);  /* Delay 10 msec */
        
        if ((u16Duty <= 0) && (u8GoUp == false))
        {
            DEBUG_PF("(%d) Up u16Duty = %d\n",u32Times,u16Duty);
            u32Times++;
            u8GoUp = true;
        }
        
        if ((u16Duty >= gts_2K_hz_Config.u16Duty) && (u8GoUp == true ))
        {
            DEBUG_PF("(%d) Down u16Duty = %d\n",u32Times,u16Duty);
            u32Times++;
            u8GoUp = false;
        }
        
        HAL_PWM_Duty_Set(USER_PWM_GROUP,u16Duty);
        
        if (u8GoUp)
            u16Duty += 128;
        else
            u16Duty -= 128;
        
        /*
        HAL_PWM_Duty_Get(USER_PWM_GROUP,&u16Get);
        DEBUG_PF("%d \n",u16Get);
        */
    }
}

