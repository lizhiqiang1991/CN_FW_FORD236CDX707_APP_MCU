/* include HAL */

#include "TimerApp.h"
#include "GpioApp.h"
#include "AdcApp.h"



#define PIN_U301_VBATT_SENSE_ADC    ADC0_LOGIC_CH0    //ADC0_0

#define PIN_THERMAL_N_L_ADC         ADC0_LOGIC_CH3  //ADC0_3
#define PIN_THERMAL_P_L_ADC         ADC0_LOGIC_CH4  //ADC0_4

#define PIN_984_TEMP_ADC            ADC1_LOGIC_CH17  // ADC1_17

#define PIN_THERMAL_P_R_ADC         ADC2_LOGIC_CH6  // ADC2_6 
#define PIN_THERMAL_N_R_ADC         ADC2_LOGIC_CH7  // ADC2_7

#define ADC_VREFH                   ADC2_LOGIC_CH0  // ADC2_0
#define ADC_VREFL                   ADC2_LOGIC_CH1  // ADC2_1

static uint16_t uAdcApp_BatteryVoltAdcValue = NUM_ZERO;

static uint16_t uADC_THERMAL_N_L_ADC = NUM_ZERO;
static uint16_t uADC_THERMAL_P_L_ADC = NUM_ZERO;

static uint16_t uADC_THERMAL_N_R_ADC = NUM_ZERO;
static uint16_t uADC_THERMAL_P_R_ADC = NUM_ZERO;

static uint16_t uBacklightNtcAdcVaules_L = NUM_ZERO;
static uint16_t uBacklightNtcAdcVaules_R = NUM_ZERO;
static uint16_t u984_TempAdcVaules = NUM_ZERO;
// static uint16_t uVoltage = NUM_ZERO;
static adc_ch_stattus_t statusBuff ;


static uint16_t uVRefHAdcVaules = NUM_ZERO;
static uint16_t uVRefLAdcVaules = NUM_ZERO;
static uint16_t uADC_1V_AdcValues =NUM_ZERO;

void Adc_Strategy(void* context)
{
    UNUSED (context);
    
    static uint8_t ADC_State = 0;
    uint16_t   u16Result = 0u;
    switch(ADC_State) 
    {
    case 0:
        /* Get the result(s) */
        HAL_ADC_Channel_Result_Get (PIN_U301_VBATT_SENSE_ADC,&u16Result, &statusBuff);       
        if (statusBuff.valid == true)
        {
            // float v = (5.0 *((float)u16Result / 4095.0));
            uAdcApp_BatteryVoltAdcValue = u16Result - uVRefLAdcVaules;
            // uVoltage = (ADC_MCU_VOL*uAdcApp_BatteryVoltAdcValue*781U) / (ADC_12BIT_ADC_MAX*100U);
        }
        
        u16Result = 0U;
        HAL_ADC_Channel_Result_Get (ADC_VREFH,&u16Result, &statusBuff);       
        if (statusBuff.valid == true)
        {
            // float v = (5.0 *((float)u16Result / 4095.0));
            uVRefHAdcVaules = u16Result;
        }
        
        u16Result = 0U;
        HAL_ADC_Channel_Result_Get (ADC_VREFL,&u16Result, &statusBuff);       
        if (statusBuff.valid == true)
        {
            // float v = (5.0 *((float)u16Result / 4095.0));
            uVRefLAdcVaules = u16Result;
        }
        
        /* Trigger next conversion */
        /** 如果設定 ADC_TRIGGER_CONTINUOUS 不需要每次執行 HAL_ADC_Channel_SW_Trigger*/
        if (ADC_TRIGGER_OFF == gtdADC_ChannelConfig.u8TriggerSelection)
        {
            HAL_ADC_Channel_SW_Trigger(PIN_984_TEMP_ADC);      
        }
        ADC_State = 1;
        break;
        
    case 1:
        /* Get the result(s) */
        HAL_ADC_Channel_Result_Get (PIN_984_TEMP_ADC,&u16Result, &statusBuff);       
        if (statusBuff.valid == true)
        {
            // float v = (5.0 *((float)u16Result / 4095.0));
            u984_TempAdcVaules = u16Result - uVRefLAdcVaules;
        }
        
        
        /* Trigger next conversion */
        /** 如果設定 ADC_TRIGGER_CONTINUOUS 不需要每次執行 HAL_ADC_Channel_SW_Trigger*/
        if (ADC_TRIGGER_OFF == gtdADC_ChannelConfig.u8TriggerSelection)
        {
            HAL_ADC_Channel_SW_Trigger(PIN_THERMAL_N_L_ADC);
            HAL_ADC_Channel_SW_Trigger(PIN_THERMAL_P_L_ADC);  
        }
        ADC_State = 2;
        break;
        
    case 2:
        /* Get the result(s) */
        HAL_ADC_Channel_Result_Get (PIN_THERMAL_N_L_ADC,&u16Result, &statusBuff);       
        if (statusBuff.valid == true)
        {
            // float v = (5.0 *((float)u16Result / 4095.0));
            uADC_THERMAL_N_L_ADC = u16Result;
        }
        u16Result = 0U;
        HAL_ADC_Channel_Result_Get (PIN_THERMAL_P_L_ADC,&u16Result, &statusBuff);       
        if (statusBuff.valid == true)
        {
            // float v = (5.0 *((float)u16Result / 4095.0));
            uADC_THERMAL_P_L_ADC = u16Result;
        }
        uBacklightNtcAdcVaules_L =  uADC_THERMAL_P_L_ADC -  uADC_THERMAL_N_L_ADC;
        
        /* Trigger next conversion */
        /** 如果設定 ADC_TRIGGER_CONTINUOUS 不需要每次執行 HAL_ADC_Channel_SW_Trigger*/
        if (ADC_TRIGGER_OFF == gtdADC_ChannelConfig.u8TriggerSelection)
        {
            HAL_ADC_Channel_SW_Trigger(PIN_THERMAL_P_R_ADC);
            HAL_ADC_Channel_SW_Trigger(PIN_THERMAL_N_R_ADC);     
        }
        uBacklightNtcAdcVaules_R = uADC_THERMAL_P_R_ADC - uADC_THERMAL_N_R_ADC;
        
        ADC_State = 3;
        
        break;
        
    case 3:
        /* Get the result(s) */
        HAL_ADC_Channel_Result_Get (PIN_THERMAL_P_R_ADC,&u16Result, &statusBuff);       
        if (statusBuff.valid == true)
        {
            // float v = (5.0 *((float)u16Result / 4095.0));
            uADC_THERMAL_P_R_ADC = u16Result;
        }
        u16Result = 0U;
        HAL_ADC_Channel_Result_Get (PIN_THERMAL_N_R_ADC,&u16Result, &statusBuff);       
        if (statusBuff.valid == true)
        {
            // float v = (5.0 *((float)u16Result / 4095.0));
            uADC_THERMAL_N_R_ADC = u16Result;
        }        
        /* Trigger next conversion */
        /** 如果設定 ADC_TRIGGER_CONTINUOUS 不需要每次執行 HAL_ADC_Channel_SW_Trigger*/
        if (ADC_TRIGGER_OFF == gtdADC_ChannelConfig.u8TriggerSelection)
        {
            HAL_ADC_Channel_SW_Trigger(PIN_U301_VBATT_SENSE_ADC);    
            HAL_ADC_Channel_SW_Trigger(ADC_VREFH);    
            HAL_ADC_Channel_SW_Trigger(ADC_VREFL);    
        }
        uBacklightNtcAdcVaules_R = uADC_THERMAL_P_R_ADC - uADC_THERMAL_N_R_ADC;
        
        ADC_State = 0;
        
        break;
        
    default:
        ADC_State = 0u;
        break;
    }
}

uint16_t AdcApp_BatteryVolAdcValue_Get (void)
{
    return uAdcApp_BatteryVoltAdcValue;
}

uint16_t AdcApp_984TEMPAdcValue_Get (void)
{
    return u984_TempAdcVaules;
}

uint16_t AdcApp_BacklightNtcAdcVaules_L_Get (void)
{
    return uBacklightNtcAdcVaules_L;
}

uint16_t AdcApp_BacklightNtcAdcVaules_R_Get (void)
{
    return uBacklightNtcAdcVaules_R;
}


uint16_t AdcApp_Ref_1V_Get (void)
{
    uADC_1V_AdcValues = ((uVRefHAdcVaules - uVRefLAdcVaules)*ADC_FIXEDPOINT_MODIFY_PARAMETER) / ADC_MCU_VOL;
    return uADC_1V_AdcValues;
}


void AdcApp_2msTimerProcess_Install(void)
{
    /* Create ADC_DET Timer for 10ms */
    HAL_Timer_Init(ADC_DET_TIMER_2MINSEC,&gts_2mSec_Timer_Continuous_Config);  //2ms
    
    /*Timer register callback*/
    HAL_Timer_Callback_Register (ADC_DET_TIMER_2MINSEC,Adc_Strategy,NULL);
    
    /*Timer Enable*/
    HAL_Timer_Active(ADC_DET_TIMER_2MINSEC  ,FUNC_ENABLE);
    
    /*ADC_DET TIMER_2MINSEC timer start*/ 
    HAL_Timer_Start(ADC_DET_TIMER_2MINSEC   ,FUNC_ENABLE);
    
    /*Enable ADC_DET TIMER_2MINSEC interrupt & recounting */
    HAL_Timer_Interrupt_Set (ADC_DET_TIMER_2MINSEC, FUNC_ENABLE, CYHAL_TCPWM_IRQ_PRIORITY);    
}
void Adc_IO_init(void)
{
    gpio_pin_config_t adcPinConfig =
    {
        .u8OutValue    = 0ul,
        .u8DriveMode = CY_GPIO_DM_ANALOG,
        .u8HSIOM     = CY_ADC_POT_PIN_MUX,
        .u8Vtrip     = 0ul,
        .u8SlewRate  = 0ul,
        .u8DriveStength  = 0ul,
    };
    
    HAL_GPIO_Pin_Init(PIN_U301_VBATT_SENSE,&adcPinConfig);
    HAL_GPIO_Pin_Init(PIN_THERMAL_N_L,  &adcPinConfig);
    HAL_GPIO_Pin_Init(PIN_THERMAL_P_L,  &adcPinConfig);
    HAL_GPIO_Pin_Init(PIN_THERMAL_N_R,  &adcPinConfig);
    HAL_GPIO_Pin_Init(PIN_THERMAL_P_R,  &adcPinConfig);
    HAL_GPIO_Pin_Init(PIN_984_TEMP,  &adcPinConfig);        
}

void Adc_Initialize(void) 
{
    uint8_t u8HalResult  = 0U;  
    Adc_IO_init();
    
    u8HalResult = HAL_ADC_Init(ADC_GROPU0,&gtdADC_Config);
    u8HalResult = HAL_ADC_Init(ADC_GROPU1,&gtdADC_Config);
    u8HalResult = HAL_ADC_Init(ADC_GROPU2,&gtdADC_Config);
#if 0    
    /* Set reference buffered mode on - to pump Vbg from SRSS */
    Cy_Adc_SetReferenceBufferMode(PASS0_EPASS_MMIO, CY_ADC_REF_BUF_MODE_ON);
    /* Read and update the raw values for VBG and Temp Sensor */
    //     adcChannelConfig.calibrationValueSelect = CY_ADC_CALIBRATION_VALUE_ALTERNATE;
#endif
    
    /*Specify ADC channel pin*/
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_AN0; 
    
    /* Initialize ADC Ch (ADC0_LOGIC_CH0) and Enable */
    /* 指定 ADC0 腳位 ADC_PIN_AN0 到 ADC0 [LOGIC CHANNEL 0 ]*/
    u8HalResult = HAL_ADC_Channel_Init(PIN_U301_VBATT_SENSE_ADC, &gtdADC_ChannelConfig);
    u8HalResult = HAL_ADC_Channel_Active (PIN_U301_VBATT_SENSE_ADC, FUNC_ENABLE);    
    /* Issue SW trigger A/D conversion */
    //u8HalResult = HAL_ADC_Channel_SW_Trigger(PIN_U301_VBATT_SENSE_ADC); 
    
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_AN3; 
    u8HalResult = HAL_ADC_Channel_Init(PIN_THERMAL_N_L_ADC, &gtdADC_ChannelConfig);
    u8HalResult = HAL_ADC_Channel_Active (PIN_THERMAL_N_L_ADC, FUNC_ENABLE);    
    /* Issue SW trigger A/D conversion */
    //u8HalResult = HAL_ADC_Channel_SW_Trigger(PIN_THERMAL_N_L_ADC); 
    
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_AN4; 
    u8HalResult = HAL_ADC_Channel_Init(PIN_THERMAL_P_L_ADC, &gtdADC_ChannelConfig);
    u8HalResult = HAL_ADC_Channel_Active (PIN_THERMAL_P_L_ADC, FUNC_ENABLE);    
    /* Issue SW trigger A/D conversion */
    //u8HalResult = HAL_ADC_Channel_SW_Trigger(PIN_THERMAL_P_L_ADC); 
    
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_AN17; 
    u8HalResult = HAL_ADC_Channel_Init(PIN_984_TEMP_ADC, &gtdADC_ChannelConfig);   
    u8HalResult = HAL_ADC_Channel_Active (PIN_984_TEMP_ADC, FUNC_ENABLE);    
    /* Issue SW trigger A/D conversion */
    //u8HalResult = HAL_ADC_Channel_SW_Trigger(PIN_984_TEMP_ADC); 
    
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_AN6; 
    u8HalResult = HAL_ADC_Channel_Init(PIN_THERMAL_P_R_ADC, &gtdADC_ChannelConfig);
    u8HalResult = HAL_ADC_Channel_Active (PIN_THERMAL_P_R_ADC, FUNC_ENABLE);    
    /* Issue SW trigger A/D conversion */
    //u8HalResult = HAL_ADC_Channel_SW_Trigger(PIN_THERMAL_P_R_ADC);    
    
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_AN7; 
    u8HalResult = HAL_ADC_Channel_Init(PIN_THERMAL_N_R_ADC, &gtdADC_ChannelConfig);
    u8HalResult = HAL_ADC_Channel_Active (PIN_THERMAL_N_R_ADC, FUNC_ENABLE);    
    /* Issue SW trigger A/D conversion */
    //u8HalResult = HAL_ADC_Channel_SW_Trigger(PIN_THERMAL_N_R_ADC);  
    
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_VREF_H; 
    u8HalResult = HAL_ADC_Channel_Init(ADC_VREFH, &gtdADC_ChannelConfig);
    u8HalResult = HAL_ADC_Channel_Active (ADC_VREFH, FUNC_ENABLE);    
    /* Issue SW trigger A/D conversion */
    //u8HalResult = HAL_ADC_Channel_SW_Trigger(ADC_VREFH);  
    
    gtdADC_ChannelConfig.u8PinAddress = ADC_PIN_VREF_L; 
    u8HalResult = HAL_ADC_Channel_Init(ADC_VREFL, &gtdADC_ChannelConfig);
    u8HalResult = HAL_ADC_Channel_Active (ADC_VREFL, FUNC_ENABLE);    
    /* Issue SW trigger A/D conversion */
    //u8HalResult = HAL_ADC_Channel_SW_Trigger(ADC_VREFL);  
    
    u8HalResult = HAL_ADC_Channel_SW_Trigger(ADC_VREFH);  
    u8HalResult = HAL_ADC_Channel_SW_Trigger(ADC_VREFL);
    u8HalResult = HAL_ADC_Channel_SW_Trigger(PIN_U301_VBATT_SENSE_ADC); 
    if(u8HalResult != DRIVER_TRUE)
    {
        ;
    }
    
}


