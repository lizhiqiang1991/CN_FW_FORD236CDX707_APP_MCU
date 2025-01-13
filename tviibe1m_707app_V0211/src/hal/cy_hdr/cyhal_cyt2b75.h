#ifndef	CYHAL_CYT2B75_H
#define CYHAL_CYT2B75_H

/** Please Refer to datasheet CYT2B6 Clock Diagram*/
#define FLL_PATH_NO          (0U)  /*CLK_PATH0 --> FLL*/
#define PLL_PATH_NO          (1U)  /*CLK_PTAH1 --> PLL*/
#define BYPASSED_PATH_NO     (2U)  /*CLK_PATH2 --> BYPASS FLL&PLL */

/** Create generic name for the series/package specific type. */
/* typedef cyhal_gpio_cyt2b75 cyhal_gpio_t; */

/**
       CYT2B6 SCB (I2C/UART/SPI) 
       Up to eight runtime-reconfigurable serial communication
       blocks (SCBs), each configurable as I2C, SPI, or UART
*/
#define CYHAL_SCB0   (0U)
#define CYHAL_SCB1   (1U)
#define CYHAL_SCB2   (2U)
#define CYHAL_SCB3   (3U)
#define CYHAL_SCB4   (4U)
#define CYHAL_SCB5   (5U)
#define CYHAL_SCB6   (6U)
#define CYHAL_SCB7   (7U)
#define CYHAL_NUM_OF_SCB (8U) 
/**
	CYT2B6 Divider

	Divider  | Count | Description
	=====================================================================================
	div_8    | 32    | Integer divider, 8 bits
	div_16   | 16    | Integer divider, 16 bits
	div_24_5 | 8     | Fractional divider, 24.5 bits (24integer bits, 5 fractional bits)
*/
#define CYHAL_DIV_NUM_ADC_16_BIT            (15U)  /* CLOCK 16-BIT DIV used in HAL ADC*/
#define CYHAL_DIV_NUM_TCPWM_COUNTER_24_BIT  (7U)   /* CLOCK 24-BIT DIV used in HAL Counter (millisecond)*/
#define CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT   (2U)   /* CLOCK 8-BIT DIV used in HAL Counter  (microsecond)*/
#define CYHAL_DIV_NUM_TCPWM_PWM_8_BIT       (3U)   /* CLOCK 8-BIT DIV used in HAL PWM*/
#define CYHAL_NUM_OF_24_5_DIV               (8U)
/**
        CYT2B6 TCPWM
        Description                     |  Instance
        =====================================================================================
        GROUP #0 (16-bit)               |    46  
        GROUP #1 (16-bit,Motor control) |    4
        GROUP #2 (32-bit)               |    2
*/
#define TCPWM_GROUP0_CNT0    (0U)
#define TCPWM_GROUP0_CNT1    (1U)
#define TCPWM_GROUP0_CNT2    (2U)
#define TCPWM_GROUP0_CNT4    (4U)
#define TCPWM_GROUP0_CNT5    (5U)
#define TCPWM_GROUP0_CNT6    (6U)
#define TCPWM_GROUP0_CNT7    (7U)
#define TCPWM_GROUP0_CNT9    (9U)
#define TCPWM_GROUP0_CNT10   (10U)
#define TCPWM_GROUP0_CNT11   (11U)
#define TCPWM_GROUP0_CNT12   (12U)
#define TCPWM_GROUP0_CNT13   (13U)
#define TCPWM_GROUP0_CNT14   (14U)
#define TCPWM_GROUP0_CNT15   (15U)
#define TCPWM_GROUP0_CNT16   (16U)
#define TCPWM_GROUP0_CNT17   (17U)
#define TCPWM_GROUP0_CNT18   (18U)
#define TCPWM_GROUP0_CNT19   (19U)
#define TCPWM_GROUP0_CNT20   (20U)
#define TCPWM_GROUP0_CNT21   (21U)
#define TCPWM_GROUP0_CNT22   (22U)
#define TCPWM_GROUP0_CNT23   (23U)
#define TCPWM_GROUP0_CNT24   (24U)
#define TCPWM_GROUP0_CNT25   (25U)
#define TCPWM_GROUP0_CNT26   (26U)
#define TCPWM_GROUP0_CNT33   (33U)
#define TCPWM_GROUP0_CNT34   (34U)
#define TCPWM_GROUP0_CNT36   (36U)
#define TCPWM_GROUP0_CNT37   (37U)
#define TCPWM_GROUP0_CNT38   (38U)
#define TCPWM_GROUP0_CNT39   (39U)
#define TCPWM_GROUP0_CNT40   (40U)
#define TCPWM_GROUP0_CNT41   (41U)
#define TCPWM_GROUP0_CNT42   (42U)
#define TCPWM_GROUP0_CNT44   (44U)
#define TCPWM_GROUP0_CNT45   (45U)
#define TCPWM_GROUP0_CNT46   (46U)
#define TCPWM_GROUP0_CNT47   (47U)
#define TCPWM_GROUP0_CNT48   (48U)
#define TCPWM_GROUP0_CNT49   (49U)
#define TCPWM_GROUP0_CNT50   (50U)
#define TCPWM_GROUP0_CNT51   (51U)
#define TCPWM_GROUP0_CNT52   (52U)
#define TCPWM_GROUP0_CNT53   (53U)
#define TCPWM_GROUP0_CNT54   (54U)
#define TCPWM_GROUP0_CNT55   (55U)
#define CYHAL_NUM_OF_TCPWM   (56U) 

/**
        CYT2B6 External interrupt
        Eight Cortex-M4/M0 external interrupts (irq 0 ~ 7)
*/
/*
#define CPUIntIdx0_IRQn  !!! CPUIntIdx0_IRQn / CPUIntIdx1_IRQn for SYSTEM Call don't use
#define CPUIntIdx1_IRQn  !!! CPUIntIdx0_IRQn / CPUIntIdx1_IRQn for SYSTEM Call don't use
#define CPUIntIdx2_IRQn
#define CPUIntIdx3_IRQn
*/
#define CYHAL_ADC_IRQn       CPUIntIdx4_IRQn
#define CYHAL_SCB_IRQn       CPUIntIdx5_IRQn
#define CYHAL_TCPWM_IRQn     CPUIntIdx6_IRQn
#define CYHAL_CPU_WDT_IRQn   CPUIntIdx7_IRQn

/**
        CYT2B6 External interrupt priority
        CM0 : External Interrupt (IRQ0~7) Configurable (0 ~ 3)
        CM4 : External interrupt (IRQ0~7) Configurable (0 ~ 7)
        !!! PRIORITY 0 and 1 for SYSTEM Call don't use
*/
#if CY_CPU_CORTEX_M4
#define CYHAL_ADC_IRQ_PRIORITY        (6U)
#define CYHAL_SCB_IRQ_PRIORITY        (6U)
#define CYHAL_TCPWM_IRQ_PRIORITY      (6U)
#else
#define CYHAL_ADC_IRQ_PRIORITY        (3u)
#define CYHAL_SCB_IRQ_PRIORITY        (3U)
#define CYHAL_TCPWM_IRQ_PRIORITY      (3U)
#endif  /*CY_CPU_CORTEX_M4*/


#endif


