/***************************************************************************
*\file hal_adc.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_adc_def.h"
#include "hal_adc.h"
 
#define DIV_ROUND_UP(a,b) (((a) + (b)/2) / (b))

/*
    Please Refer to CYT2B6 datasheet to check 
    SpecID : SID114A , ADC operating frequency 2 – 13.34 MHz (Max) 2.7 V ≤ VDDA < 4.5 V
*/
#define ADC_OPERATION_FREQUENCY_MAX_IN_HZ (13340000ul)

/*
    SpecID :SID113A , Analog input sample time 600ns (Min) 2.7 V ≤ VDDA < 4.5 V
    Guaranteed by design
*/
#define ANALOG_IN_SAMPLING_TIME_MIN_IN_NS (600ull)
                                                 
typedef struct 
{
   volatile stc_PASS_SAR_t* pSARn;
   const cy_en_intr_t eIntr;
   const uint8_t u8LogicChannel;
   HAL_ADC_CALLBACK  pCallback;
}hal_adc_channel;

static hal_adc_channel tdADC0[] =
{
#if defined(tviibe1m)  
  { PASS0_SAR0, pass_0_interrupts_sar_0_IRQn , 0u, NULL}, /* pass_0_interrupts_sar_0_IRQn   =  84 ADC #0, Channel #0 */
  { PASS0_SAR0, pass_0_interrupts_sar_1_IRQn , 1u, NULL}, /* pass_0_interrupts_sar_1_IRQn   =  85 ADC #0, Channel #1 */
  { PASS0_SAR0, pass_0_interrupts_sar_2_IRQn , 2u, NULL}, /* pass_0_interrupts_sar_2_IRQn   =  86 ADC #0, Channel #2 */
  { PASS0_SAR0, pass_0_interrupts_sar_3_IRQn , 3u, NULL}, /* pass_0_interrupts_sar_3_IRQn   =  87 ADC #0, Channel #3 */
  { PASS0_SAR0, pass_0_interrupts_sar_4_IRQn , 4u, NULL}, /* pass_0_interrupts_sar_4_IRQn   =  88 ADC #0, Channel #4 */
  { PASS0_SAR0, pass_0_interrupts_sar_5_IRQn , 5u, NULL}, /* pass_0_interrupts_sar_5_IRQn   =  89 ADC #0, Channel #5 */
  { PASS0_SAR0, pass_0_interrupts_sar_6_IRQn , 6u, NULL}, /* pass_0_interrupts_sar_6_IRQn   =  90 ADC #0, Channel #6 */
  { PASS0_SAR0, pass_0_interrupts_sar_7_IRQn , 7u, NULL}, /* pass_0_interrupts_sar_7_IRQn   =  91 ADC #0, Channel #7 */
  { PASS0_SAR0, pass_0_interrupts_sar_8_IRQn , 8u, NULL}, /* pass_0_interrupts_sar_8_IRQn   =  92 ADC #0, Channel #8 */
  { PASS0_SAR0, pass_0_interrupts_sar_9_IRQn , 9u, NULL}, /* pass_0_interrupts_sar_9_IRQn   =  93 ADC #0, Channel #9 */
  { PASS0_SAR0, pass_0_interrupts_sar_10_IRQn, 10u,NULL}, /* pass_0_interrupts_sar_10_IRQn  =  94 ADC #0, Channel #10 */
  { PASS0_SAR0, pass_0_interrupts_sar_11_IRQn, 11u,NULL}, /* pass_0_interrupts_sar_11_IRQn  =  95 ADC #0, Channel #11 */
  { PASS0_SAR0, pass_0_interrupts_sar_12_IRQn, 12u,NULL}, /* pass_0_interrupts_sar_12_IRQn  =  96 ADC #0, Channel #12 */
  { PASS0_SAR0, pass_0_interrupts_sar_13_IRQn, 13u,NULL}, /* pass_0_interrupts_sar_13_IRQn  =  97 ADC #0, Channel #13 */
  { PASS0_SAR0, pass_0_interrupts_sar_14_IRQn, 14u,NULL}, /* pass_0_interrupts_sar_14_IRQn  =  98 ADC #0, Channel #14 */
  { PASS0_SAR0, pass_0_interrupts_sar_15_IRQn, 15u,NULL}, /* pass_0_interrupts_sar_15_IRQn  =  99 ADC #0, Channel #15 */
  { PASS0_SAR0, pass_0_interrupts_sar_16_IRQn, 16u,NULL}, /* pass_0_interrupts_sar_16_IRQn  = 100 ADC #0, Channel #16 */
  { PASS0_SAR0, pass_0_interrupts_sar_17_IRQn, 17u,NULL}, /* pass_0_interrupts_sar_17_IRQn  = 101 ADC #0, Channel #17 */
  { PASS0_SAR0, pass_0_interrupts_sar_18_IRQn, 18u,NULL}, /* pass_0_interrupts_sar_18_IRQn  = 102 ADC #0, Channel #18 */
  { PASS0_SAR0, pass_0_interrupts_sar_19_IRQn, 19u,NULL}, /* pass_0_interrupts_sar_19_IRQn  = 103 ADC #0, Channel #19 */
  { PASS0_SAR0, pass_0_interrupts_sar_20_IRQn, 20u,NULL}, /* pass_0_interrupts_sar_20_IRQn  = 104 ADC #0, Channel #20 */
  { PASS0_SAR0, pass_0_interrupts_sar_21_IRQn, 21u,NULL}, /* pass_0_interrupts_sar_21_IRQn  = 105 ADC #0, Channel #21 */
  { PASS0_SAR0, pass_0_interrupts_sar_22_IRQn, 22u,NULL}, /* pass_0_interrupts_sar_22_IRQn  = 106 ADC #0, Channel #22 */
  { PASS0_SAR0, pass_0_interrupts_sar_23_IRQn, 23u,NULL}, /* pass_0_interrupts_sar_23_IRQn  = 107 ADC #0, Channel #23 */
#elif defined(tviibe512k)
 { PASS0_SAR0, pass_0_interrupts_sar_0_IRQn , 0u, NULL}, /* pass_0_interrupts_sar_0_IRQn   =  84 ADC #0, Channel #0 */
 { PASS0_SAR0, pass_0_interrupts_sar_1_IRQn , 1u, NULL}, /* pass_0_interrupts_sar_1_IRQn   =  85 ADC #0, Channel #1 */
 { PASS0_SAR0, pass_0_interrupts_sar_2_IRQn , 2u, NULL}, /* pass_0_interrupts_sar_2_IRQn   =  86 ADC #0, Channel #2 */
 { PASS0_SAR0, pass_0_interrupts_sar_3_IRQn , 3u, NULL}, /* pass_0_interrupts_sar_3_IRQn   =  87 ADC #0, Channel #3 */
 { PASS0_SAR0, pass_0_interrupts_sar_4_IRQn , 4u, NULL}, /* pass_0_interrupts_sar_4_IRQn   =  88 ADC #0, Channel #4 */
 { PASS0_SAR0, pass_0_interrupts_sar_5_IRQn , 5u, NULL}, /* pass_0_interrupts_sar_5_IRQn   =  89 ADC #0, Channel #5 */
 {       NULL, pass_0_interrupts_sar_6_IRQn , 6u, NULL}, /* pass_0_interrupts_sar_6_IRQn   =  90 ADC #0, Channel #6 */
 {       NULL, pass_0_interrupts_sar_7_IRQn , 7u, NULL}, /* pass_0_interrupts_sar_7_IRQn   =  91 ADC #0, Channel #7 */
 { PASS0_SAR0, pass_0_interrupts_sar_8_IRQn , 8u, NULL}, /* pass_0_interrupts_sar_8_IRQn   =  92 ADC #0, Channel #8 */
 { PASS0_SAR0, pass_0_interrupts_sar_9_IRQn , 9u, NULL}, /* pass_0_interrupts_sar_9_IRQn   =  93 ADC #0, Channel #9 */
 {       NULL, pass_0_interrupts_sar_10_IRQn, 10u,NULL}, /* pass_0_interrupts_sar_10_IRQn  =  94 ADC #0, Channel #10 */
 { PASS0_SAR0, pass_0_interrupts_sar_11_IRQn, 11u,NULL}, /* pass_0_interrupts_sar_11_IRQn  =  95 ADC #0, Channel #11 */
 {       NULL, pass_0_interrupts_sar_12_IRQn, 12u,NULL}, /* pass_0_interrupts_sar_12_IRQn  =  96 ADC #0, Channel #12 */
 {       NULL, pass_0_interrupts_sar_13_IRQn, 13u,NULL}, /* pass_0_interrupts_sar_13_IRQn  =  97 ADC #0, Channel #13 */
 {       NULL, pass_0_interrupts_sar_14_IRQn, 14u,NULL}, /* pass_0_interrupts_sar_14_IRQn  =  98 ADC #0, Channel #14 */
 {       NULL, pass_0_interrupts_sar_15_IRQn, 15u,NULL}, /* pass_0_interrupts_sar_15_IRQn  =  99 ADC #0, Channel #15 */
 {       NULL, pass_0_interrupts_sar_16_IRQn, 16u,NULL}, /* pass_0_interrupts_sar_16_IRQn  = 100 ADC #0, Channel #16 */
 { PASS0_SAR0, pass_0_interrupts_sar_17_IRQn, 17u,NULL}, /* pass_0_interrupts_sar_17_IRQn  = 101 ADC #0, Channel #17 */
 {       NULL, pass_0_interrupts_sar_18_IRQn, 18u,NULL}, /* pass_0_interrupts_sar_18_IRQn  = 102 ADC #0, Channel #18 */
 {       NULL, pass_0_interrupts_sar_19_IRQn, 19u,NULL}, /* pass_0_interrupts_sar_19_IRQn  = 103 ADC #0, Channel #19 */
 {       NULL, pass_0_interrupts_sar_20_IRQn, 20u,NULL}, /* pass_0_interrupts_sar_20_IRQn  = 104 ADC #0, Channel #20 */
 {       NULL, pass_0_interrupts_sar_21_IRQn, 21u,NULL}, /* pass_0_interrupts_sar_21_IRQn  = 105 ADC #0, Channel #21 */
 {       NULL, pass_0_interrupts_sar_22_IRQn, 22u,NULL}, /* pass_0_interrupts_sar_22_IRQn  = 106 ADC #0, Channel #22 */
 {       NULL, pass_0_interrupts_sar_23_IRQn, 23u,NULL}, /* pass_0_interrupts_sar_23_IRQn  = 107 ADC #0, Channel #23 */
#endif  
};
  
static hal_adc_channel tdADC1[] =  
{
#if defined(tviibe1m)     
  { PASS0_SAR1, pass_0_interrupts_sar_32_IRQn, 0u, NULL}, /* pass_0_interrupts_sar_32_IRQn  = 108 ADC #1, Channel #0 */
  { PASS0_SAR1, pass_0_interrupts_sar_33_IRQn, 1u, NULL}, /* pass_0_interrupts_sar_33_IRQn  = 109 ADC #1, Channel #1 */
  { PASS0_SAR1, pass_0_interrupts_sar_34_IRQn, 2u, NULL}, /* pass_0_interrupts_sar_34_IRQn  = 110 ADC #1, Channel #2 */
  { PASS0_SAR1, pass_0_interrupts_sar_35_IRQn, 3u, NULL}, /* pass_0_interrupts_sar_35_IRQn  = 111 ADC #1, Channel #3 */
  { PASS0_SAR1, pass_0_interrupts_sar_36_IRQn, 4u, NULL}, /* pass_0_interrupts_sar_36_IRQn  = 112 ADC #1, Channel #4 */
  { PASS0_SAR1, pass_0_interrupts_sar_37_IRQn, 5u, NULL}, /* pass_0_interrupts_sar_37_IRQn  = 113 ADC #1, Channel #5 */
  { PASS0_SAR1, pass_0_interrupts_sar_38_IRQn, 6u, NULL}, /* pass_0_interrupts_sar_38_IRQn  = 114 ADC #1, Channel #6 */
  { PASS0_SAR1, pass_0_interrupts_sar_39_IRQn, 7u, NULL}, /* pass_0_interrupts_sar_39_IRQn  = 115 ADC #1, Channel #7 */
  { PASS0_SAR1, pass_0_interrupts_sar_40_IRQn, 8u, NULL}, /* pass_0_interrupts_sar_40_IRQn  = 116 ADC #1, Channel #8 */
  { PASS0_SAR1, pass_0_interrupts_sar_41_IRQn, 9u, NULL}, /* pass_0_interrupts_sar_41_IRQn  = 117 ADC #1, Channel #9 */
  { PASS0_SAR1, pass_0_interrupts_sar_42_IRQn, 10u,NULL}, /* pass_0_interrupts_sar_42_IRQn  = 118 ADC #1, Channel #10 */
  { PASS0_SAR1, pass_0_interrupts_sar_43_IRQn, 11u,NULL}, /* pass_0_interrupts_sar_43_IRQn  = 119 ADC #1, Channel #11 */
  { PASS0_SAR1, pass_0_interrupts_sar_44_IRQn, 12u,NULL}, /* pass_0_interrupts_sar_44_IRQn  = 120 ADC #1, Channel #12 */
  { PASS0_SAR1, pass_0_interrupts_sar_45_IRQn, 13u,NULL}, /* pass_0_interrupts_sar_45_IRQn  = 121 ADC #1, Channel #13 */
  { PASS0_SAR1, pass_0_interrupts_sar_46_IRQn, 14u,NULL}, /* pass_0_interrupts_sar_46_IRQn  = 122 ADC #1, Channel #14 */
  { PASS0_SAR1, pass_0_interrupts_sar_47_IRQn, 15u,NULL}, /* pass_0_interrupts_sar_47_IRQn  = 123 ADC #1, Channel #15 */
  { PASS0_SAR1, pass_0_interrupts_sar_48_IRQn, 16u,NULL}, /* pass_0_interrupts_sar_48_IRQn  = 124 ADC #1, Channel #16 */
  { PASS0_SAR1, pass_0_interrupts_sar_49_IRQn, 17u,NULL}, /* pass_0_interrupts_sar_49_IRQn  = 125 ADC #1, Channel #17 */
  { PASS0_SAR1, pass_0_interrupts_sar_50_IRQn, 18u,NULL}, /* pass_0_interrupts_sar_50_IRQn  = 126 ADC #1, Channel #18 */
  { PASS0_SAR1, pass_0_interrupts_sar_51_IRQn, 19u,NULL}, /* pass_0_interrupts_sar_51_IRQn  = 127 ADC #1, Channel #19 */
  { PASS0_SAR1, pass_0_interrupts_sar_52_IRQn, 20u,NULL}, /* pass_0_interrupts_sar_52_IRQn  = 128 ADC #1, Channel #20 */
  { PASS0_SAR1, pass_0_interrupts_sar_53_IRQn, 21u,NULL}, /* pass_0_interrupts_sar_53_IRQn  = 129 ADC #1, Channel #21 */
  { PASS0_SAR1, pass_0_interrupts_sar_54_IRQn, 22u,NULL}, /* pass_0_interrupts_sar_54_IRQn  = 130 ADC #1, Channel #22 */
  { PASS0_SAR1, pass_0_interrupts_sar_55_IRQn, 23u,NULL}, /* pass_0_interrupts_sar_55_IRQn  = 131 ADC #1, Channel #23 */
  { PASS0_SAR1, pass_0_interrupts_sar_56_IRQn, 24u,NULL}, /* pass_0_interrupts_sar_56_IRQn  = 132 ADC #1, Channel #24 */
  { PASS0_SAR1, pass_0_interrupts_sar_57_IRQn, 25u,NULL}, /* pass_0_interrupts_sar_57_IRQn  = 133 ADC #1, Channel #25 */
  { PASS0_SAR1, pass_0_interrupts_sar_58_IRQn, 26u,NULL}, /* pass_0_interrupts_sar_58_IRQn  = 134 ADC #1, Channel #26 */
  { PASS0_SAR1, pass_0_interrupts_sar_59_IRQn, 27u,NULL}, /* pass_0_interrupts_sar_59_IRQn  = 135 ADC #1, Channel #27 */
  { PASS0_SAR1, pass_0_interrupts_sar_60_IRQn, 28u,NULL}, /* pass_0_interrupts_sar_60_IRQn  = 136 ADC #1, Channel #28 */
  { PASS0_SAR1, pass_0_interrupts_sar_61_IRQn, 29u,NULL}, /* pass_0_interrupts_sar_61_IRQn  = 137 ADC #1, Channel #29 */
  { PASS0_SAR1, pass_0_interrupts_sar_62_IRQn, 30u,NULL}, /* pass_0_interrupts_sar_62_IRQn  = 138 ADC #1, Channel #30 */
  { PASS0_SAR1, pass_0_interrupts_sar_63_IRQn, 31u,NULL}, /* pass_0_interrupts_sar_63_IRQn  = 139 ADC #1, Channel #31 */
#elif defined(tviibe512k)
 {       NULL, pass_0_interrupts_sar_32_IRQn, 0u, NULL}, /* pass_0_interrupts_sar_32_IRQn  = 108 ADC #1, Channel #0 */
 {       NULL, pass_0_interrupts_sar_33_IRQn, 1u, NULL}, /* pass_0_interrupts_sar_33_IRQn  = 109 ADC #1, Channel #1 */
 {       NULL, pass_0_interrupts_sar_34_IRQn, 2u, NULL}, /* pass_0_interrupts_sar_34_IRQn  = 110 ADC #1, Channel #2 */
 {       NULL, pass_0_interrupts_sar_35_IRQn, 3u, NULL}, /* pass_0_interrupts_sar_35_IRQn  = 111 ADC #1, Channel #3 */
 { PASS0_SAR1, pass_0_interrupts_sar_36_IRQn, 4u, NULL}, /* pass_0_interrupts_sar_36_IRQn  = 112 ADC #1, Channel #4 */
 { PASS0_SAR1, pass_0_interrupts_sar_37_IRQn, 5u, NULL}, /* pass_0_interrupts_sar_37_IRQn  = 113 ADC #1, Channel #5 */
 { PASS0_SAR1, pass_0_interrupts_sar_38_IRQn, 6u, NULL}, /* pass_0_interrupts_sar_38_IRQn  = 114 ADC #1, Channel #6 */
 { PASS0_SAR1, pass_0_interrupts_sar_39_IRQn, 7u, NULL}, /* pass_0_interrupts_sar_39_IRQn  = 115 ADC #1, Channel #7 */
 { PASS0_SAR1, pass_0_interrupts_sar_40_IRQn, 8u, NULL}, /* pass_0_interrupts_sar_40_IRQn  = 116 ADC #1, Channel #8 */
 {       NULL, pass_0_interrupts_sar_41_IRQn, 9u, NULL}, /* pass_0_interrupts_sar_41_IRQn  = 117 ADC #1, Channel #9 */
 {       NULL, pass_0_interrupts_sar_42_IRQn, 10u,NULL}, /* pass_0_interrupts_sar_42_IRQn  = 118 ADC #1, Channel #10 */
 {       NULL, pass_0_interrupts_sar_43_IRQn, 11u,NULL}, /* pass_0_interrupts_sar_43_IRQn  = 119 ADC #1, Channel #11 */
 { PASS0_SAR1, pass_0_interrupts_sar_44_IRQn, 12u,NULL}, /* pass_0_interrupts_sar_44_IRQn  = 120 ADC #1, Channel #12 */
 { PASS0_SAR1, pass_0_interrupts_sar_45_IRQn, 13u,NULL}, /* pass_0_interrupts_sar_45_IRQn  = 121 ADC #1, Channel #13 */
 { PASS0_SAR1, pass_0_interrupts_sar_46_IRQn, 14u,NULL}, /* pass_0_interrupts_sar_46_IRQn  = 122 ADC #1, Channel #14 */
 { PASS0_SAR1, pass_0_interrupts_sar_47_IRQn, 15u,NULL}, /* pass_0_interrupts_sar_47_IRQn  = 123 ADC #1, Channel #15 */
 { PASS0_SAR1, pass_0_interrupts_sar_48_IRQn, 16u,NULL}, /* pass_0_interrupts_sar_48_IRQn  = 124 ADC #1, Channel #16 */
 { PASS0_SAR1, pass_0_interrupts_sar_49_IRQn, 17u,NULL}, /* pass_0_interrupts_sar_49_IRQn  = 125 ADC #1, Channel #17 */
 {       NULL, pass_0_interrupts_sar_50_IRQn, 18u,NULL}, /* pass_0_interrupts_sar_50_IRQn  = 126 ADC #1, Channel #18 */
 {       NULL, pass_0_interrupts_sar_51_IRQn, 19u,NULL}, /* pass_0_interrupts_sar_51_IRQn  = 127 ADC #1, Channel #19 */
 {       NULL, pass_0_interrupts_sar_52_IRQn, 20u,NULL}, /* pass_0_interrupts_sar_52_IRQn  = 128 ADC #1, Channel #20 */
 {       NULL, pass_0_interrupts_sar_53_IRQn, 21u,NULL}, /* pass_0_interrupts_sar_53_IRQn  = 129 ADC #1, Channel #21 */
 { PASS0_SAR1, pass_0_interrupts_sar_54_IRQn, 22u,NULL}, /* pass_0_interrupts_sar_54_IRQn  = 130 ADC #1, Channel #22 */
 { PASS0_SAR1, pass_0_interrupts_sar_55_IRQn, 23u,NULL}, /* pass_0_interrupts_sar_55_IRQn  = 131 ADC #1, Channel #23 */
 {       NULL, pass_0_interrupts_sar_56_IRQn, 24u,NULL}, /* pass_0_interrupts_sar_56_IRQn  = 132 ADC #1, Channel #24 */
 {       NULL, pass_0_interrupts_sar_57_IRQn, 25u,NULL}, /* pass_0_interrupts_sar_57_IRQn  = 133 ADC #1, Channel #25 */
 {       NULL, pass_0_interrupts_sar_58_IRQn, 26u,NULL}, /* pass_0_interrupts_sar_58_IRQn  = 134 ADC #1, Channel #26 */
 {       NULL, pass_0_interrupts_sar_59_IRQn, 27u,NULL}, /* pass_0_interrupts_sar_59_IRQn  = 135 ADC #1, Channel #27 */
 {       NULL, pass_0_interrupts_sar_60_IRQn, 28u,NULL}, /* pass_0_interrupts_sar_60_IRQn  = 136 ADC #1, Channel #28 */
 {       NULL, pass_0_interrupts_sar_61_IRQn, 29u,NULL}, /* pass_0_interrupts_sar_61_IRQn  = 137 ADC #1, Channel #29 */
 {       NULL, pass_0_interrupts_sar_62_IRQn, 30u,NULL}, /* pass_0_interrupts_sar_62_IRQn  = 138 ADC #1, Channel #30 */
 {       NULL, pass_0_interrupts_sar_63_IRQn, 31u,NULL}, /* pass_0_interrupts_sar_63_IRQn  = 139 ADC #1, Channel #31 */
#endif  
};

static hal_adc_channel tdADC2[] = 
{
  { PASS0_SAR2, pass_0_interrupts_sar_64_IRQn, 0u, NULL}, /* pass_0_interrupts_sar_64_IRQn  = 140 ADC #2, Channel #0 */
  { PASS0_SAR2, pass_0_interrupts_sar_65_IRQn, 1u, NULL}, /* pass_0_interrupts_sar_65_IRQn  = 141 ADC #2, Channel #1 */
  { PASS0_SAR2, pass_0_interrupts_sar_66_IRQn, 2u, NULL}, /* pass_0_interrupts_sar_66_IRQn  = 142 ADC #2, Channel #2 */
  { PASS0_SAR2, pass_0_interrupts_sar_67_IRQn, 3u, NULL}, /* pass_0_interrupts_sar_67_IRQn  = 143 ADC #2, Channel #3 */
  { PASS0_SAR2, pass_0_interrupts_sar_68_IRQn, 4u, NULL}, /* pass_0_interrupts_sar_68_IRQn  = 144 ADC #2, Channel #4 */
  { PASS0_SAR2, pass_0_interrupts_sar_69_IRQn, 5u, NULL}, /* pass_0_interrupts_sar_69_IRQn  = 145 ADC #2, Channel #5 */
  { PASS0_SAR2, pass_0_interrupts_sar_70_IRQn, 6u, NULL}, /* pass_0_interrupts_sar_70_IRQn  = 146 ADC #2, Channel #6 */
  { PASS0_SAR2, pass_0_interrupts_sar_71_IRQn, 7u, NULL}, /* pass_0_interrupts_sar_71_IRQn  = 147 ADC #2, Channel #7 */
};
 

static const cy_stc_adc_channel_config_t tsAdcChannelDefConfig =
{
    .triggerSelection          = CY_ADC_TRIGGER_OFF, //CY_ADC_TRIGGER_CONTINUOUS, //CY_ADC_TRIGGER_OFF,
    .channelPriority           = 0u,
    .preenptionType            = CY_ADC_PREEMPTION_FINISH_RESUME,
    .isGroupEnd                = false,
    .doneLevel                 = CY_ADC_DONE_LEVEL_PULSE,
    .pinAddress                = CY_ADC_PIN_ADDRESS_AN0,
    .portAddress               = CY_ADC_PORT_ADDRESS_SARMUX0,
    .extMuxSelect              = 0u,
    .extMuxEnable              = true,
    .preconditionMode          = CY_ADC_PRECONDITION_MODE_OFF,
    .overlapDiagMode           = CY_ADC_OVERLAP_DIAG_MODE_OFF,
    .sampleTime                = 0u,
    .calibrationValueSelect    = CY_ADC_CALIBRATION_VALUE_REGULAR,
    .postProcessingMode        = CY_ADC_POST_PROCESSING_MODE_NONE,
    .resultAlignment           = CY_ADC_RESULT_ALIGNMENT_RIGHT,
    .signExtention             = CY_ADC_SIGN_EXTENTION_UNSIGNED,
    .averageCount              = 0u,
    .rightShift                = 0u,
    .rangeDetectionMode        = CY_ADC_RANGE_DETECTION_MODE_INSIDE_RANGE,
    .rangeDetectionLoThreshold = 0x0000u,
    .rangeDetectionHiThreshold = 0x0FFFu,
    .mask.grpDone              = false,
    .mask.grpCancelled         = false,
    .mask.grpOverflow          = false,
    .mask.chRange              = false,
    .mask.chPulse              = false,
    .mask.chOverflow           = false,
};

static uint32_t u32SamplingCycle = 0u;

static volatile stc_PASS_SAR_t* ADC_Group_Get( uint8_t u8ADCGroup)
{
    volatile stc_PASS_SAR_t* pBase = NULL;
    volatile stc_PASS_SAR_t* pGroup[NUM_OF_ADC_GROUP] =  {PASS0_SAR0,PASS0_SAR1,PASS0_SAR2};
    if (u8ADCGroup < NUM_OF_ADC_GROUP)
        pBase = pGroup[u8ADCGroup];
    else
        pBase = NULL;
    return pBase;
}

#define CASE_ADGROUP(x)  case ADC_GROPU ##x: \
{\
    if (u8LogicChannel <= sizeof(tdADC ##x)/sizeof(tdADC ##x[0u]))\
    {\
        pCh = &tdADC ##x[u8LogicChannel];\
    }\
    else\
    {\
        pCh = NULL;\
    }\
} break

static hal_adc_channel* ADC_Channel_Get( uint8_t u8ADC_Channel)
{
    hal_adc_channel* pCh;
    uint8_t u8ADCGroup      = CYHAL_GET_ADC_PORT((u8ADC_Channel));
    uint8_t u8LogicChannel  = CYHAL_GET_ADC_CHANNEL((u8ADC_Channel));
    
    switch (u8ADCGroup)
    {
      CASE_ADGROUP(0);
      CASE_ADGROUP(1);
      CASE_ADGROUP(2);
       
      default:
        pCh = NULL;
        break;
    } /*End of Switch*/
    
    if (NULL != pCh)
    {
        if (NULL == pCh->pSARn)
           pCh =  NULL;
    }
    else
    {
        pCh =  NULL;
    }
    
    return pCh;
}

static en_clk_dst_t ADC_Group_Clock_Dst_Get(uint8_t u8ADCGroup)
{
    en_clk_dst_t eADC_Clk;
    switch(u8ADCGroup)
    {
      case ADC_GROPU0:
        eADC_Clk = PCLK_PASS0_CLOCK_SAR0;
        break;
      case ADC_GROPU1:
        eADC_Clk = PCLK_PASS0_CLOCK_SAR1;
        break;
      case ADC_GROPU2:
        eADC_Clk = PCLK_PASS0_CLOCK_SAR2;
        break;
      default:
        eADC_Clk = PCLK_CPUSS_CLOCK_TRACE_IN;
        break;
    }
    return eADC_Clk;
}

static void ADC_Clock_Set(en_clk_dst_t eADC_ClkDst)
{
    Cy_SysClk_PeriphAssignDivider(eADC_ClkDst, CY_SYSCLK_DIV_16_BIT, CYHAL_DIV_NUM_ADC_16_BIT);
      
    if (0u == u32SamplingCycle)
    {
        uint32_t u32PeriFreq = 0;
        uint32_t u32DivNum = 0;
    
        /* ADC clock divider setting */
        Cy_SysClk_GetClkPeriFrequency(&u32PeriFreq);
        u32DivNum = DIV_ROUND_UP(u32PeriFreq, ADC_OPERATION_FREQUENCY_MAX_IN_HZ);
    
        Cy_SysClk_PeriphSetDivider(CY_SYSCLK_DIV_16_BIT, CYHAL_DIV_NUM_ADC_16_BIT, (u32DivNum - 1ul));
        Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_16_BIT, CYHAL_DIV_NUM_ADC_16_BIT);
        uint32_t u32ActualAdcOperationFreq = u32PeriFreq / u32DivNum;
        u32SamplingCycle = (uint32_t)DIV_ROUND_UP((ANALOG_IN_SAMPLING_TIME_MIN_IN_NS * (uint64_t)u32ActualAdcOperationFreq), 1000000000ull);
    }
}

uint8_t HAL_ADC_Init(uint8_t u8ADCGroup, adc_group_config_t* tsConfig)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    volatile stc_PASS_SAR_t* pADC = ADC_Group_Get(u8ADCGroup);
    if ((NULL == pADC) || (NULL == tsConfig))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        cy_stc_adc_config_t tsAdcConfig =
        {
            .preconditionTime    = 0u,
            .powerupTime         = 0u,
            .enableIdlePowerDown = false,
            .msbStretchMode      = CY_ADC_MSB_STRETCH_MODE_1CYCLE,
            .enableHalfLsbConv   = 0u,
            .sarMuxEnable        = true,
            .adcEnable           = true,
            .sarIpEnable         = true,
        };
        tsAdcConfig.preconditionTime = tsConfig->u8PreconditionTime;
        tsAdcConfig.powerupTime      = tsConfig->u8PowerupTime;
        if (CY_ADC_SUCCESS == Cy_Adc_Init(pADC, &tsAdcConfig))
        {
            en_clk_dst_t eADC_Clk = ADC_Group_Clock_Dst_Get(u8ADCGroup);
            if (PCLK_CPUSS_CLOCK_TRACE_IN != eADC_Clk)
            {
                ADC_Clock_Set( eADC_Clk );
                u8HalResult = DRIVER_TRUE;
            }
            else
            {
                u8HalResult = DRIVER_PARAMETER_ERROR;
            }
        }
        else
            u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_ADC_Channel_Init (uint8_t u8ADC_Channel, adc_channel_config_t* tsChannelConfig)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    
    hal_adc_channel* pADC_Ch = ADC_Channel_Get( u8ADC_Channel );
    if ((NULL == pADC_Ch) || (NULL == tsChannelConfig))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        volatile stc_PASS_SAR_t* pSAR = pADC_Ch->pSARn;
        uint8_t u8LogicChannel = pADC_Ch->u8LogicChannel;
        
        cy_stc_adc_channel_config_t tsAdcChannelConfig;
        /* Initialize ADC channel configuration */
        memcpy(&tsAdcChannelConfig,&tsAdcChannelDefConfig,sizeof(cy_stc_adc_channel_config_t));
        tsAdcChannelConfig.triggerSelection = (cy_en_adc_trigger_selection_t) tsChannelConfig->u8TriggerSelection;
        /* Address of the analog signal (pin) to be sampled by corresponding channel*/
        tsAdcChannelConfig.pinAddress = (cy_en_adc_pin_address_t) tsChannelConfig->u8PinAddress;   
        /* This value indicates that this channel is the last channel of a group or not */
        tsAdcChannelConfig.isGroupEnd = tsChannelConfig->u8IsGroupEnd;
        
        if (true == tsAdcChannelConfig.isGroupEnd)
            tsAdcChannelConfig.mask.grpDone = true;
        else
            tsAdcChannelConfig.mask.grpDone = false;
        
        /*  Sample time (aperture) in ADC clock cycles. Minimum is 1 (0 gives the same result as 1),
        *  minimum time needed for proper settling is at least 300ns,
        *  i.e. 6 clock cycles at the max frequency of 20MHz.*/
        tsAdcChannelConfig.sampleTime = u32SamplingCycle;
        /* Select whether result data is signed or unsigned. */
        tsAdcChannelConfig.signExtention = (cy_en_adc_sign_extention_t) tsChannelConfig->u8SignExtention; 
        /* Select range detection mode*/
        tsAdcChannelConfig.rangeDetectionMode = (cy_en_adc_range_detection_mode_t) tsChannelConfig->u8RangeDetectionMode;        
        /* Range detect low threshold (Lo) */
        tsAdcChannelConfig.rangeDetectionLoThreshold = tsChannelConfig->u16RangeDetectionLoThreshold; 
        /* Range detect high threshold (Hi) */
        tsAdcChannelConfig.rangeDetectionHiThreshold = tsChannelConfig->u16RangeDetectionHiThreshold; 
        
        if (CY_ADC_SUCCESS == Cy_Adc_Channel_Init(&pSAR->CH[u8LogicChannel], &tsAdcChannelConfig))
            u8HalResult = DRIVER_TRUE;
        else
            u8HalResult = DRIVER_PARAMETER_ERROR;  
        
    }
    return u8HalResult;
}

uint8_t HAL_ADC_Channel_Active (uint8_t u8ADC_Channel, uint8_t u8Enable)
{
    uint8_t u8HalResult = DRIVER_FALSE;
     
    hal_adc_channel* pADC_ch = ADC_Channel_Get( u8ADC_Channel );
    if ( (NULL != pADC_ch) &&\
         (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable) )
    {
        volatile stc_PASS_SAR_t* pSAR = pADC_ch->pSARn;
        uint8_t u8LogicChannel = pADC_ch->u8LogicChannel;
        
        if (FUNC_ENABLE == u8Enable)
            Cy_Adc_Channel_Enable(&pSAR->CH[u8LogicChannel]);
        else
            Cy_Adc_Channel_Disable(&pSAR->CH[u8LogicChannel]);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}


uint8_t HAL_ADC_Channel_SW_Trigger(uint8_t u8ADC_Channel)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    hal_adc_channel* pADC_ch = ADC_Channel_Get( u8ADC_Channel );
      
    if (NULL == pADC_ch)
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        volatile stc_PASS_SAR_t* pSAR = pADC_ch->pSARn;
        uint8_t u8LogicChannel = pADC_ch->u8LogicChannel;
        /* Issue SW trigger */
        Cy_Adc_Channel_SoftwareTrigger(&pSAR->CH[u8LogicChannel]);
        u8HalResult = DRIVER_TRUE;
    }
    return u8HalResult;
}

uint8_t HAL_ADC_Channel_Result_Get (uint8_t u8ADC_Channel, uint16_t* pResult, adc_ch_stattus_t* tsStatus)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    hal_adc_channel* pADC_ch = ADC_Channel_Get( u8ADC_Channel );
    if ( (NULL == pADC_ch) || (NULL == pResult) || (NULL == tsStatus) )
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        volatile stc_PASS_SAR_t* pSAR = pADC_ch->pSARn;
        uint8_t u8LogicChannel = pADC_ch->u8LogicChannel;
        /* Get the result(s) */
        if (CY_ADC_SUCCESS == Cy_Adc_Channel_GetResult(&pSAR->CH[u8LogicChannel], pResult, tsStatus))
            u8HalResult = DRIVER_TRUE;
        else
            u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

static void HAL_UNDEF_ADC_IRQ(void)
{
 ;
}

static void HAL_ADC_IRQ(void)
{
    cy_stc_adc_interrupt_source_t intrSource = { false };
    cy_en_intr_t eInterrupt_src = Cy_SysInt_GetIntStatus(CYHAL_ADC_IRQn);
    
    uint16_t u16Index,u16LogicCh;
    volatile stc_PASS_SAR_t* pSAR = NULL;
    HAL_ADC_CALLBACK pCallback = NULL;
    hal_adc_channel* pADC_Ch;
    if ( (eInterrupt_src >= pass_0_interrupts_sar_0_IRQn) &&\
         (eInterrupt_src <= pass_0_interrupts_sar_71_IRQn) )
    {
        if (eInterrupt_src < pass_0_interrupts_sar_32_IRQn)
        {
            /*ADC 0*/
            u16Index = eInterrupt_src - pass_0_interrupts_sar_0_IRQn;
            pADC_Ch = tdADC0;
        } 
        else if (eInterrupt_src < pass_0_interrupts_sar_64_IRQn)
        {
            /*ADC 1*/
            u16Index = eInterrupt_src - pass_0_interrupts_sar_32_IRQn;
            pADC_Ch = tdADC1;
        }
        else
        {
            /*ADC 2*/
            u16Index = eInterrupt_src - pass_0_interrupts_sar_64_IRQn;
            pADC_Ch = tdADC2;
        }
        
        pSAR       = pADC_Ch[u16Index].pSARn;
        u16LogicCh = pADC_Ch[u16Index].u8LogicChannel;
        pCallback  = pADC_Ch[u16Index].pCallback;
        
        if (NULL != pSAR)
        {
            Cy_Adc_Channel_GetInterruptMaskedStatus(&pSAR->CH[u16LogicCh], &intrSource);
            
            if (NULL != pCallback)
                (*pCallback) (intrSource.grpDone);
            
            if (true == intrSource.grpDone)
            {
                /* Clear interrupt source(s) (Only last channel is required) */
                Cy_Adc_Channel_ClearInterruptStatus(&pSAR->CH[u16LogicCh], &intrSource);
            }    
        }   
    }
    else
    {
        HAL_UNDEF_ADC_IRQ();
    }
    
#if 0
    for (uint16_t i = 0 ; i < sizeof(tdADCallback)/sizeof(tdADCallback[0u]) ; i++)
    {
        if ((tdADCallback[i].eIntr == eInterrupt_src) && (tdADCallback[i].pADC != NULL))
        {
            /* Get interrupt source */
            Cy_Adc_Channel_GetInterruptMaskedStatus(&tdADCallback[i].pADC->CH[tdADCallback[i].u8LogicChannel], &intrSource);
            
            (*tdADCallback[i].pCallback) (intrSource.grpDone);
            
            if (true == intrSource.grpDone)
            {
                /* Clear interrupt source(s) (Only last channel is required) */
                Cy_Adc_Channel_ClearInterruptStatus(&tdADCallback[i].pADC->CH[tdADCallback[i].u8LogicChannel], &intrSource);
            }
        }
    }
#endif  
}

uint8_t HAL_ADC_Callback_Register (uint8_t u8ADC_Channel, HAL_ADC_CALLBACK pCallback)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    hal_adc_channel* pADC_Ch = ADC_Channel_Get(u8ADC_Channel);
    
    if ((NULL == pADC_Ch) || (NULL == pCallback))
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        cy_stc_sysint_irq_t tsIrq_cfg = 
        {
            .sysIntSrc  = disconnected_IRQn, 
            .intIdx     = CYHAL_ADC_IRQn,
            .isEnabled  = true,
        };
       
        tsIrq_cfg.sysIntSrc = pADC_Ch->eIntr;
        Cy_SysInt_InitIRQ(&tsIrq_cfg);
        Cy_SysInt_SetSystemIrqVector(tsIrq_cfg.sysIntSrc, HAL_ADC_IRQ);
        
        /** Set ADC callback context*/
        pADC_Ch->pCallback = pCallback;
        
        /** Enable ADC irq*/
        NVIC_SetPriority(CYHAL_ADC_IRQn, CYHAL_ADC_IRQ_PRIORITY);
        NVIC_ClearPendingIRQ(CYHAL_ADC_IRQn);
        NVIC_EnableIRQ(CYHAL_ADC_IRQn);
        u8HalResult = DRIVER_TRUE;
    }
    
    return u8HalResult;
}

/*** End of file ***/

  