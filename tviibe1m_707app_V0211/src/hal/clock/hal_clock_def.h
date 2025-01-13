#ifndef HAL_CLOCK_DEF_H
#define HAL_CLOCK_DEF_H

typedef struct {
    uint8_t  u8InputClockPath; /*CY_SYSCLK_CLKPATH_IN_IMO / CY_SYSCLK_CLKPATH_IN_ECO / CY_SYSCLK_CLKPATH_IN_EXT*/
    uint32_t u32InClkFreq;
    uint32_t u32OutClkFreq;
} pll_fll_config_t;

typedef struct {
    pll_fll_config_t tsPllConfig;
    pll_fll_config_t tsFllConfig;
    uint32_t u32EcoFreq;
    uint8_t u8EcoEnable;
    uint8_t u8WcoEnable;
    uint8_t u8Ilo0Enable;
    uint8_t u8Ilo1Enable;
    uint32_t u32Timeout;
} clock_config_t;

/** Wait time definition **/
#define WAIT_FOR_STABILIZATION (10000U)
  
#define PLL_IN_ECO   (CY_SYSCLK_CLKPATH_IN_ECO)  
#define PLL_IN_IMO   (CY_SYSCLK_CLKPATH_IN_IMO) 
#define PLL_IN_EXT   (CY_SYSCLK_CLKPATH_IN_EXT)
#define FLL_IN_ECO   (PLL_IN_ECO)  
#define FLL_IN_IMO   (PLL_IN_IMO)  
#define FLL_IN_EXT   (PLL_IN_EXT)

#define CLOCK_PATH0   FLL_PATH_NO
#define CLOCK_PATH1   PLL_PATH_NO
#define CLOCK_PATH2   BYPASSED_PATH_NO

#define CLOCK_HF0         (0U)
#define CLOCK_HF1         (1U)
/*
#define CLOCK_HF2         xxx
#define CLOCK_HF3         xxx
*/

/** ECO parameters **/
#define CLK_FREQ_ECO            (16000000UL)
#define LOAD_CAP_IN_PF          (17UL)
#define ESR_IN_OHME             (250UL)
#define MAX_DRIVE_LEVEL_IN_UW   (100UL)

extern clock_config_t gtsClockInit;

#endif


