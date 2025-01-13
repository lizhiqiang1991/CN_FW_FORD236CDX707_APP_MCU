/***************************************************************************
*\file hal_pwm_def.h 
****************************************************************************/

#ifndef HAL_PWM_DEF_H
#define HAL_PWM_DEF_H

/**
        Maximum count	65535 (16BIT PWM)
        Minimum count	32
        
        PWM clock base 80Mhz
        
        DIV = 1 , 80Mhz
        output range 2k ~ 4M hz
        
        DIV = 2 , 40Mhz
        output range 610 ~ 1250K hz
        
        DIV = 4 , 20Mhz
        output range 305 ~ 625K hz
        
        DIV = 8 , 10Mhz
        output range 153 ~ 312.5K hz
        
        DIV = 16 , 5Mhz
        output range 73 ~ 156.25K hz
        
        DIV = 32 , 2.5Mhz
        output range 38 ~ 78.125K hz
        
        DIV = 64 , 1.25Mhz
        output range 19 ~ 39,063 hz
        
        DIV = 128
        output range 10 ~ 19,531 hz
	
	
For example : PWM left alignment 

|-----------|   
|           | 	
            ------------
<- Compare->
<-         Period      ->

Ex1 : 
    Peri clock (CLK_HF0) = 80Mhz
    u8PreScaler = 2 , 80Mhz/2 = 40Mhz
    Period = 1000   , 40Mhz / 1000 = 40Khz 
    Compare = 500   , 50% PWM duty (500/1000)
    PWM OUTPUT => 40Khz , 50% duty 

Ex2 :
    Peri clock (CLK_HF0) = 80Mhz
    u8PreScaler = 4 , 80Mhz/4 = 20Mhz
    Period = 20,000 , 20Mhz / 20,000 = 1Khz 
    Compare = 6000  , 30% PWM duty (6000/20,000)
    PWM OUTPUT => 1Khz , 30% duty 
*/

/*PWM Prescaler (u8PreScaler)*/
#define PWM_PRESCALER_DIVBY_1   CY_TCPWM_PWM_PRESCALER_DIVBY_1   /**< Divide by 1 */
#define PWM_PRESCALER_DIVBY_2   CY_TCPWM_PWM_PRESCALER_DIVBY_2   /**< Divide by 2 */
#define PWM_PRESCALER_DIVBY_4   CY_TCPWM_PWM_PRESCALER_DIVBY_4   /**< Divide by 4 */
#define PWM_PRESCALER_DIVBY_8   CY_TCPWM_PWM_PRESCALER_DIVBY_8   /**< Divide by 8 */
#define PWM_PRESCALER_DIVBY_16  CY_TCPWM_PWM_PRESCALER_DIVBY_16  /**< Divide by 16 */
#define PWM_PRESCALER_DIVBY_32  CY_TCPWM_PWM_PRESCALER_DIVBY_32  /**< Divide by 32 */
#define PWM_PRESCALER_DIVBY_64  CY_TCPWM_PWM_PRESCALER_DIVBY_64  /**< Divide by 64 */
#define PWM_PRESCALER_DIVBY_128 CY_TCPWM_PWM_PRESCALER_DIVBY_128 /**< Divide by 128 */

/*PWM Alignment (u8Mode)*/
#define PWM_LEFT_ALIGN          CY_TCPWM_PWM_LEFT_ALIGN    /**< PWM is left aligned, meaning it starts high */
#define PWM_RIGHT_ALIGN         CY_TCPWM_PWM_RIGHT_ALIGN   /**< PWM is right aligned meaning it start low */
#define PWM_CENTER_ALIGN        CY_TCPWM_PWM_CENTER_ALIGN  /**< PWM is centered aligned, terminal count only occurs on underflow */

/*PWM RUN TYPE (u8RunType)*/
#define PWM_ONESHOT             CY_TCPWM_PWM_ONESHOT     /**< Counter runs once and then stops */
#define PWM_CONTINUOUS          CY_TCPWM_PWM_CONTINUOUS  /**< Counter runs forever */

/*PWM Maximum and Minimun count*/
#define PWM_MAX_COUNT  (65535U)
#define PWM_MIN_COUNT  (0U)

/*PWM Configuration*/
typedef struct
{
    /*Peri clock 80Mhz*/
    uint8_t u8PreScaler;        /**< 1/2/4/8/16/32/64/128 */
    uint8_t u8Pin;              /**< pwm output pin*/
    uint8_t u8PinComplementary; /**< An optional, additional inverted output pin.*/
    uint16_t u16Period;         /**< 0 ~ 65535*/
    uint16_t u16Duty;           /**< 0 ~ 65535 Compare */
    uint8_t u8Mode;             /**< PWM alignment */
    uint8_t u8RunType;          /**< Continuous or One Shot*/ 
    uint8_t u8Invert;           /**< An option for the user to invert the PWM output*/
    uint8_t u8InterruptEnable;  /**< Enable/Disable Interrupt*/
    uint8_t u8NvicPriority;     /**< ISR priority*/
    uint32_t u32Timeout;        /**< PWM Config timeout */
}pwm_config_t;

typedef void(*PWM_ISR) (void *pContext);

/*Redefine TCPWM GROUP it again, they are the same hardware unit*/
#define PWM_GROUP0_CNT0    TCPWM_GROUP0_CNT0 
#define PWM_GROUP0_CNT1    TCPWM_GROUP0_CNT1 
#define PWM_GROUP0_CNT2    TCPWM_GROUP0_CNT2 
#define PWM_GROUP0_CNT4    TCPWM_GROUP0_CNT4 
#define PWM_GROUP0_CNT5    TCPWM_GROUP0_CNT5 
#define PWM_GROUP0_CNT6    TCPWM_GROUP0_CNT6 
#define PWM_GROUP0_CNT7    TCPWM_GROUP0_CNT7 
#define PWM_GROUP0_CNT9    TCPWM_GROUP0_CNT9 
#define PWM_GROUP0_CNT10   TCPWM_GROUP0_CNT10
#define PWM_GROUP0_CNT11   TCPWM_GROUP0_CNT11
#define PWM_GROUP0_CNT12   TCPWM_GROUP0_CNT12
#define PWM_GROUP0_CNT13   TCPWM_GROUP0_CNT13
#define PWM_GROUP0_CNT14   TCPWM_GROUP0_CNT14
#define PWM_GROUP0_CNT15   TCPWM_GROUP0_CNT15
#define PWM_GROUP0_CNT16   TCPWM_GROUP0_CNT16
#define PWM_GROUP0_CNT17   TCPWM_GROUP0_CNT17
#define PWM_GROUP0_CNT18   TCPWM_GROUP0_CNT18
#define PWM_GROUP0_CNT19   TCPWM_GROUP0_CNT19
#define PWM_GROUP0_CNT20   TCPWM_GROUP0_CNT20
#define PWM_GROUP0_CNT21   TCPWM_GROUP0_CNT21
#define PWM_GROUP0_CNT22   TCPWM_GROUP0_CNT22
#define PWM_GROUP0_CNT23   TCPWM_GROUP0_CNT23
#define PWM_GROUP0_CNT24   TCPWM_GROUP0_CNT24
#define PWM_GROUP0_CNT25   TCPWM_GROUP0_CNT25
#define PWM_GROUP0_CNT26   TCPWM_GROUP0_CNT26
#define PWM_GROUP0_CNT33   TCPWM_GROUP0_CNT33
#define PWM_GROUP0_CNT34   TCPWM_GROUP0_CNT34
#define PWM_GROUP0_CNT36   TCPWM_GROUP0_CNT36
#define PWM_GROUP0_CNT37   TCPWM_GROUP0_CNT37
#define PWM_GROUP0_CNT38   TCPWM_GROUP0_CNT38
#define PWM_GROUP0_CNT39   TCPWM_GROUP0_CNT39
#define PWM_GROUP0_CNT40   TCPWM_GROUP0_CNT40
#define PWM_GROUP0_CNT41   TCPWM_GROUP0_CNT41
#define PWM_GROUP0_CNT42   TCPWM_GROUP0_CNT42
#define PWM_GROUP0_CNT44   TCPWM_GROUP0_CNT44
#define PWM_GROUP0_CNT45   TCPWM_GROUP0_CNT45
#define PWM_GROUP0_CNT46   TCPWM_GROUP0_CNT46
#define PWM_GROUP0_CNT47   TCPWM_GROUP0_CNT47
#define PWM_GROUP0_CNT48   TCPWM_GROUP0_CNT48
#define PWM_GROUP0_CNT49   TCPWM_GROUP0_CNT49
#define PWM_GROUP0_CNT50   TCPWM_GROUP0_CNT50
#define PWM_GROUP0_CNT51   TCPWM_GROUP0_CNT51
#define PWM_GROUP0_CNT52   TCPWM_GROUP0_CNT52
#define PWM_GROUP0_CNT53   TCPWM_GROUP0_CNT53
#define PWM_GROUP0_CNT54   TCPWM_GROUP0_CNT54
#define PWM_GROUP0_CNT55   TCPWM_GROUP0_CNT55

extern pwm_config_t gts_2K_hz_Config; 

#endif

  