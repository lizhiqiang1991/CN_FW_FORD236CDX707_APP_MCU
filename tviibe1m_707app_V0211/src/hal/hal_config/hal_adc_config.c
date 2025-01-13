/***************************************************************************
*\file hal_adc_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_adc_def.h"

/*ADC Group configutaion*/
adc_group_config_t gtdADC_Config =
{
    .u8PreconditionTime = 0u,
    .u8PowerupTime = 0u,
};

/*ADC Channel configuration*/
adc_channel_config_t gtdADC_ChannelConfig = 
{
    .u8PinAddress = ADC_PIN_AN0, /** channel 0*/
    .u8IsGroupEnd = true,        /**< This value indicates that this channel is the last channel of a group or not */
    .u8TriggerSelection = ADC_TRIGGER_OFF,
    .u8SignExtention = ADC_SIGN_EXTENTION_UNSIGNED, /** Select whether result data is signed or unsigned. */
    .u8RangeDetectionMode = ADC_RANGE_DETECTION_MODE_INSIDE_RANGE, /**< Select range detection mode*/
    .u16RangeDetectionLoThreshold = 0x0000u, /**< Range detect low threshold (Lo) */
    .u16RangeDetectionHiThreshold = 0x0FFFu, /**< Range detect high threshold (Hi) */
};
 
/*** End of file ***/

  