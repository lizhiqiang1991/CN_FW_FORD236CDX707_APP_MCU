/*

File    : testbed_gpio.c
Purpose : GPIO testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include "hal_gpio_def.h"
#include "CYTVII_B_E_1M_KIT_PIN.h"

#define USE_TOOGLE_FUNCTION  1
 
void GPIO_Test_Toggle(void);
void GPIO_Test_HI_LOW(void);

#ifdef tviibh8m
  #define USER_LED            IO_P23_2
  #define USER_BUTTON         IO_P21_4
#else
  #define USER_LED_1         KIT_LED_1_BLUE 
  #define USER_LED_4         KIT_LED_4_BLUE 

  #define USER_LED           USER_LED_1
  #define USER_BUTTON        KIT_BUTTON_PIN    /*SW1 CYTVII-B-E-1M-SK*/
#endif

static void port_config()
{
#if 1
  gpio_port_config_t tsPortConfig =
  {
    .u32OutValue = PIN_LOW, /**< Initial output data for the IO pins in the port */
    .u32DriveMode = 0x0U,   /**< Port pin drive modes and input buffer enable configuration */
    .u32CfgIn = 0U,         /**< Port pin input buffer configuration */
    .u32CfgOut = 0U,        /**< Port pin output buffer configuration */
    .u32HSIOM0 = 0U,        /**< HSIOM selection for port pins 0,1,2,3 */
    .u32HSIOM1 = 0U,        /**< HSIOM selection for port pins 4,5,6,7 */
  };
  
  /*Port drive modes config */
  /*
       0000  0000 0000 0000 0000 0000 0000 0000   <-- u32DriveMode binary 32bits
       Pin7  Pin6 Pin5 Pin4 Pin3 Pin2 Pin1 Pin0
  */
  /*Set Pin0 & Pin1 driver mode (GPIO OUTPUT)*/
  tsPortConfig.u32DriveMode = CY_GPIO_DM_STRONG_IN_OFF << 4 |CY_GPIO_DM_STRONG_IN_OFF << 0;
  /*Init port 12*/
  HAL_GPIO_Port_Init(IO_PORT12,&tsPortConfig);
  
#else
  
 HAL_GPIO_Pin_Init(IO_P12_0,&gtdPinOutputConfig);  
 HAL_GPIO_Pin_Init(IO_P12_1,&gtdPinOutputConfig); 
  
#endif
  
}
void testbed_gpio(void) 
{
  __enable_irq();
  
  /*LED GPIO config as output*/
  HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
 
  /*BUTTON GPIO config as input*/
  HAL_GPIO_Pin_Init(USER_BUTTON,&gtdPinInputConfig);
  
  /*Tesing GPIO port*/
  port_config();
  /*GPIO port output*/
  uint32_t u32PortMask = 0x03; /*pin 0 & pin 1 b'0000 0011*/
  HAL_GPIO_PortWrite(IO_PORT12,u32PortMask,PIN_HIGH);
  
  uint8_t u8Button = 0;
  for (;;)
  {
#if USE_TOOGLE_FUNCTION
    GPIO_Test_Toggle();
#else
    GPIO_Test_HI_LOW();
#endif
    
    HAL_GPIO_PinRead(USER_BUTTON,&u8Button);
    //printf(" Button Status %d\n",u8Button);
  }
}

void GPIO_Test_Toggle(void)
{
  HAL_GPIO_PinToggle(USER_LED);
  DELAY(1600000U);
}

void GPIO_Test_HI_LOW(void)
{
  HAL_GPIO_PinWrite(USER_LED,PIN_HIGH);
  DELAY(1600000U);
  HAL_GPIO_PinWrite(USER_LED,PIN_LOW);
  DELAY(1600000U);
}

/*************************** End of file ****************************/
