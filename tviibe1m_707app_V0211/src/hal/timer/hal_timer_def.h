/***************************************************************************
*\file hal_timer_def.h 
****************************************************************************/

#ifndef HAL_TIMER_DEF_H
#define HAL_TIMER_DEF_H

/**
        CYT2B6 TCPWM
        Description                     |  Instance
        ==================================================================
        GROUP #0 (16-bit)               |    46  
        GROUP #1 (16-bit,Motor control) |    4
        GROUP #2 (32-bit)               |    2
*/

#define TIMER_GROUP0_CNT0    TCPWM_GROUP0_CNT0 
#define TIMER_GROUP0_CNT1    TCPWM_GROUP0_CNT1 
#define TIMER_GROUP0_CNT2    TCPWM_GROUP0_CNT2 
#define TIMER_GROUP0_CNT4    TCPWM_GROUP0_CNT4 
#define TIMER_GROUP0_CNT5    TCPWM_GROUP0_CNT5 
#define TIMER_GROUP0_CNT6    TCPWM_GROUP0_CNT6 
#define TIMER_GROUP0_CNT7    TCPWM_GROUP0_CNT7 
#define TIMER_GROUP0_CNT9    TCPWM_GROUP0_CNT9 
#define TIMER_GROUP0_CNT10   TCPWM_GROUP0_CNT10
#define TIMER_GROUP0_CNT11   TCPWM_GROUP0_CNT11
#define TIMER_GROUP0_CNT12   TCPWM_GROUP0_CNT12
#define TIMER_GROUP0_CNT13   TCPWM_GROUP0_CNT13
#define TIMER_GROUP0_CNT14   TCPWM_GROUP0_CNT14
#define TIMER_GROUP0_CNT15   TCPWM_GROUP0_CNT15
#define TIMER_GROUP0_CNT16   TCPWM_GROUP0_CNT16
#define TIMER_GROUP0_CNT17   TCPWM_GROUP0_CNT17
#define TIMER_GROUP0_CNT18   TCPWM_GROUP0_CNT18
#define TIMER_GROUP0_CNT19   TCPWM_GROUP0_CNT19
#define TIMER_GROUP0_CNT20   TCPWM_GROUP0_CNT20
#define TIMER_GROUP0_CNT21   TCPWM_GROUP0_CNT21
#define TIMER_GROUP0_CNT22   TCPWM_GROUP0_CNT22
#define TIMER_GROUP0_CNT23   TCPWM_GROUP0_CNT23
#define TIMER_GROUP0_CNT24   TCPWM_GROUP0_CNT24
#define TIMER_GROUP0_CNT25   TCPWM_GROUP0_CNT25
#define TIMER_GROUP0_CNT26   TCPWM_GROUP0_CNT26
#define TIMER_GROUP0_CNT33   TCPWM_GROUP0_CNT33
#define TIMER_GROUP0_CNT34   TCPWM_GROUP0_CNT34
#define TIMER_GROUP0_CNT36   TCPWM_GROUP0_CNT36
#define TIMER_GROUP0_CNT37   TCPWM_GROUP0_CNT37
#define TIMER_GROUP0_CNT38   TCPWM_GROUP0_CNT38
#define TIMER_GROUP0_CNT39   TCPWM_GROUP0_CNT39
#define TIMER_GROUP0_CNT40   TCPWM_GROUP0_CNT40
#define TIMER_GROUP0_CNT41   TCPWM_GROUP0_CNT41
#define TIMER_GROUP0_CNT42   TCPWM_GROUP0_CNT42
#define TIMER_GROUP0_CNT44   TCPWM_GROUP0_CNT44
#define TIMER_GROUP0_CNT45   TCPWM_GROUP0_CNT45
#define TIMER_GROUP0_CNT46   TCPWM_GROUP0_CNT46
#define TIMER_GROUP0_CNT47   TCPWM_GROUP0_CNT47
#define TIMER_GROUP0_CNT48   TCPWM_GROUP0_CNT48
#define TIMER_GROUP0_CNT49   TCPWM_GROUP0_CNT49
#define TIMER_GROUP0_CNT50   TCPWM_GROUP0_CNT50
#define TIMER_GROUP0_CNT51   TCPWM_GROUP0_CNT51
#define TIMER_GROUP0_CNT52   TCPWM_GROUP0_CNT52
#define TIMER_GROUP0_CNT53   TCPWM_GROUP0_CNT53
#define TIMER_GROUP0_CNT54   TCPWM_GROUP0_CNT54
#define TIMER_GROUP0_CNT55   TCPWM_GROUP0_CNT55

typedef void(*Timer_ISR) (void *pContext);

typedef struct
{
    uint32_t u32Period;  /*0~ 65535 uSec*/
    uint8_t u8RunType; /* Continuous 、One Shot*/
    uint8_t u8InterruptEnable;
    uint8_t u8NvicPriority;
    uint32_t u32Timeout;
}timer_config_t;

/*u8RunType*/
#define TIMER_CONTINUOUS  CY_TCPWM_PWM_CONTINUOUS
#define TIMER_ONESHOT     CY_TCPWM_PWM_ONESHOT

/*Timer config_t example*/
extern timer_config_t gts_1mSec_Timer_Continuous_Config;
extern timer_config_t gts_1mSec_Timer_OneShot_Config;
extern timer_config_t gts_10mSec_Timer_Continuous_Config;
extern timer_config_t gts_2mSec_Timer_Continuous_Config;
extern timer_config_t gts_25mSec_Timer_Continuous_Config;
extern timer_config_t gts_50mSec_Timer_Continuous_Config;
#endif

  