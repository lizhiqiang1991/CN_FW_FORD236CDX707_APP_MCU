/***************************************************************************
*\file hal_adc_def.h 
****************************************************************************/

#ifndef HAL_ADC_DEF_H
#define HAL_ADC_DEF_H

/** Gets a ADC definition from the provided ADC port and channel numbers */
#define CYHAL_GET_ADC(port, channel)   ((((uint8_t)(port)) << 6U) + ((uint8_t)(channel)))

/** Macro that, given a adc, will extract the channel number */
#define CYHAL_GET_ADC_CHANNEL(channel)          ((uint8_t)(((uint8_t)channel) & 0x3FU))

/** Macro that, given a ADC, will extract the port number */
#define CYHAL_GET_ADC_PORT(channel)         ((uint8_t)((((uint8_t)channel) >> 6U) & 0x03U))

/*ADC Group Configuration*/
typedef struct
{
    uint8_t u8PreconditionTime;    /**< Number ADC clock cycles that Preconditioning is done before the sample window starts. */
    uint8_t u8PowerupTime;         /**< Number cycles to wait to power up after IDLE_PWRDWN. */ 
}adc_group_config_t;

/*ADC Channel Configuration*/
typedef struct
{
    uint8_t  u8PinAddress;                 /**< Address of the analog signal (pin) to be sampled by corresponding channel*/
    bool     u8IsGroupEnd;                 /**< This value indicates that this channel is the last channel of a group or not */
    uint8_t  u8TriggerSelection;           /**< The ADC trigger mode selection. ADC_TRIGGER_OFF or ADC_TRIGGER_CONTINUOUS*/
    uint8_t  u8SignExtention;              /** Select whether result data is signed or unsigned. */
    uint8_t  u8RangeDetectionMode;         /**< Select range detection mode*/
    uint16_t u16RangeDetectionLoThreshold; /**< Range detect low threshold (Lo) */
    uint16_t u16RangeDetectionHiThreshold; /**< Range detect high threshold (Hi) */
} adc_channel_config_t;

typedef cy_stc_adc_ch_status_t adc_ch_stattus_t;
typedef void(*HAL_ADC_CALLBACK) (bool u8GroupDone);

/*ADC Group */
#define ADC_GROPU0   (0u)
#define ADC_GROPU1   (1u)
#define ADC_GROPU2   (2u)
#define NUM_OF_ADC_GROUP (3u)

/** (u8TriggerSelection)*/
#define ADC_TRIGGER_OFF          CY_ADC_TRIGGER_OFF
#define ADC_TRIGGER_CONTINUOUS   CY_ADC_TRIGGER_CONTINUOUS

/** (u8SignExtention) Select whether result data is signed or unsigned. */
#define ADC_SIGN_EXTENTION_UNSIGNED CY_ADC_SIGN_EXTENTION_UNSIGNED /**< Result data is unsigned (zero extended if needed). */
#define ADC_SIGN_EXTENTION_SIGNED   CY_ADC_SIGN_EXTENTION_SIGNED   /**< Result data is signed (sign extended if needed). */
 
/** (u8PinAddress) ADC Pin/Channel*/
#define ADC_PIN_AN0        CY_ADC_PIN_ADDRESS_AN0       /**<  Vout = AN0, select corresponding analog input. */
#define ADC_PIN_AN1        CY_ADC_PIN_ADDRESS_AN1       
#define ADC_PIN_AN2        CY_ADC_PIN_ADDRESS_AN2       
#define ADC_PIN_AN3        CY_ADC_PIN_ADDRESS_AN3       
#define ADC_PIN_AN4        CY_ADC_PIN_ADDRESS_AN4       
#define ADC_PIN_AN5        CY_ADC_PIN_ADDRESS_AN5       
#define ADC_PIN_AN6        CY_ADC_PIN_ADDRESS_AN6       
#define ADC_PIN_AN7        CY_ADC_PIN_ADDRESS_AN7       
#define ADC_PIN_AN8        CY_ADC_PIN_ADDRESS_AN8       
#define ADC_PIN_AN9        CY_ADC_PIN_ADDRESS_AN9       
#define ADC_PIN_AN10       CY_ADC_PIN_ADDRESS_AN10      
#define ADC_PIN_AN11       CY_ADC_PIN_ADDRESS_AN11      
#define ADC_PIN_AN12       CY_ADC_PIN_ADDRESS_AN12      
#define ADC_PIN_AN13       CY_ADC_PIN_ADDRESS_AN13      
#define ADC_PIN_AN14       CY_ADC_PIN_ADDRESS_AN14      
#define ADC_PIN_AN15       CY_ADC_PIN_ADDRESS_AN15      
#define ADC_PIN_AN16       CY_ADC_PIN_ADDRESS_AN16      
#define ADC_PIN_AN17       CY_ADC_PIN_ADDRESS_AN17      
#define ADC_PIN_AN18       CY_ADC_PIN_ADDRESS_AN18      
#define ADC_PIN_AN19       CY_ADC_PIN_ADDRESS_AN19      
#define ADC_PIN_AN20       CY_ADC_PIN_ADDRESS_AN20      
#define ADC_PIN_AN21       CY_ADC_PIN_ADDRESS_AN21      
#define ADC_PIN_AN22       CY_ADC_PIN_ADDRESS_AN22      
#define ADC_PIN_AN23       CY_ADC_PIN_ADDRESS_AN23      
#define ADC_PIN_AN24       CY_ADC_PIN_ADDRESS_AN24      
#define ADC_PIN_AN25       CY_ADC_PIN_ADDRESS_AN25      
#define ADC_PIN_AN26       CY_ADC_PIN_ADDRESS_AN26      
#define ADC_PIN_AN27       CY_ADC_PIN_ADDRESS_AN27      
#define ADC_PIN_AN28       CY_ADC_PIN_ADDRESS_AN28      
#define ADC_PIN_AN29       CY_ADC_PIN_ADDRESS_AN29      
#define ADC_PIN_AN30       CY_ADC_PIN_ADDRESS_AN30      
#define ADC_PIN_AN31       CY_ADC_PIN_ADDRESS_AN31      
#define ADC_PIN_VMOTOR     CY_ADC_PIN_ADDRESS_VMOTOR    /**< Vout = Vmotor, select motor input. */
#define ADC_PIN_VAUX       CY_ADC_PIN_ADDRESS_VAUX      /**< Vout = Vaux, select auxiliarly input. */
#define ADC_PIN_AMUXBUS_A  CY_ADC_PIN_ADDRESS_AMUXBUS_A /**< Vout = AmuxbusA. */
#define ADC_PIN_AMUXBUS_B  CY_ADC_PIN_ADDRESS_AMUXBUS_B /**< Vout = AmuxbusB. */
#define ADC_PIN_VCCD       CY_ADC_PIN_ADDRESS_VCCD      /**< Vout = Vccd. */
#define ADC_PIN_VDDA       CY_ADC_PIN_ADDRESS_VDDA      /**< Vout = Vdda. */
#define ADC_PIN_VBG        CY_ADC_PIN_ADDRESS_VBG       /**< Vout = Vbg, Bandgap voltage from SRSS. */
#define ADC_PIN_VTEMP      CY_ADC_PIN_ADDRESS_VTEMP     /**< Vout = Vtemp, select temperature sensor. Make sure that only 1 ADC is allowed to use this.. */
#define ADC_PIN_VREF_L     CY_ADC_PIN_ADDRESS_VREF_L    /**< Vout = VrefL (VrefL actually bypasses the SARMUX (XSL)). */
#define ADC_PIN_VREF_H     CY_ADC_PIN_ADDRESS_VREF_H    /**< Vout = VrefH  (VrefH actually bypasses the SARMUX (XSH)) */

/** (u8RangeDetectionMode) Select range detection mode. */
#define ADC_RANGE_DETECTION_MODE_BELOW_LO      CY_ADC_RANGE_DETECTION_MODE_BELOW_LO      /**< Below Low threshold (result < Lo) */
#define ADC_RANGE_DETECTION_MODE_INSIDE_RANGE  CY_ADC_RANGE_DETECTION_MODE_INSIDE_RANGE  /**< Inside range (Lo <= result < Hi) */
#define ADC_RANGE_DETECTION_MODE_ABOVE_HI      CY_ADC_RANGE_DETECTION_MODE_ABOVE_HI      /**< Above high threshold (Hi <= result) */
#define ADC_RANGE_DETECTION_MODE_OUTSIDE_RANGE CY_ADC_RANGE_DETECTION_MODE_OUTSIDE_RANGE /**< Outside range (result < Lo || Hi <= result) */

/** ADC (uint8_t u8ADC_Channel) for ADC logic channel operation*/
 
/** ADC[0] */
#define ADC0_LOGIC_CH0         CYHAL_GET_ADC( ADC_GROPU0,0u ) /*!< ADC[0] logic channel 0*/
#define ADC0_LOGIC_CH1         CYHAL_GET_ADC( ADC_GROPU0,1u ) 
#define ADC0_LOGIC_CH2         CYHAL_GET_ADC( ADC_GROPU0,2u ) 
#define ADC0_LOGIC_CH3         CYHAL_GET_ADC( ADC_GROPU0,3u ) 
#define ADC0_LOGIC_CH4         CYHAL_GET_ADC( ADC_GROPU0,4u ) 
#define ADC0_LOGIC_CH5         CYHAL_GET_ADC( ADC_GROPU0,5u ) 
#define ADC0_LOGIC_CH6         CYHAL_GET_ADC( ADC_GROPU0,6u ) 
#define ADC0_LOGIC_CH7         CYHAL_GET_ADC( ADC_GROPU0,7u ) 
#define ADC0_LOGIC_CH8         CYHAL_GET_ADC( ADC_GROPU0,8u ) 
#define ADC0_LOGIC_CH9         CYHAL_GET_ADC( ADC_GROPU0,9u ) 
#define ADC0_LOGIC_CH10        CYHAL_GET_ADC( ADC_GROPU0,10u )
#define ADC0_LOGIC_CH11        CYHAL_GET_ADC( ADC_GROPU0,11u )
#define ADC0_LOGIC_CH12        CYHAL_GET_ADC( ADC_GROPU0,12u )
#define ADC0_LOGIC_CH13        CYHAL_GET_ADC( ADC_GROPU0,13u )
#define ADC0_LOGIC_CH14        CYHAL_GET_ADC( ADC_GROPU0,14u )
#define ADC0_LOGIC_CH15        CYHAL_GET_ADC( ADC_GROPU0,15u )
#define ADC0_LOGIC_CH16        CYHAL_GET_ADC( ADC_GROPU0,16u )
#define ADC0_LOGIC_CH17        CYHAL_GET_ADC( ADC_GROPU0,17u )
#define ADC0_LOGIC_CH18        CYHAL_GET_ADC( ADC_GROPU0,18u )
#define ADC0_LOGIC_CH19        CYHAL_GET_ADC( ADC_GROPU0,19u )
#define ADC0_LOGIC_CH20        CYHAL_GET_ADC( ADC_GROPU0,20u )
#define ADC0_LOGIC_CH21        CYHAL_GET_ADC( ADC_GROPU0,21u )
#define ADC0_LOGIC_CH22        CYHAL_GET_ADC( ADC_GROPU0,22u )
#define ADC0_LOGIC_CH23        CYHAL_GET_ADC( ADC_GROPU0,23u )
#define ADC0_LOGIC_CH24        CYHAL_GET_ADC( ADC_GROPU0,24u )
#define ADC0_LOGIC_CH25        CYHAL_GET_ADC( ADC_GROPU0,25u )
#define ADC0_LOGIC_CH26        CYHAL_GET_ADC( ADC_GROPU0,26u )
#define ADC0_LOGIC_CH27        CYHAL_GET_ADC( ADC_GROPU0,27u )
#define ADC0_LOGIC_CH28        CYHAL_GET_ADC( ADC_GROPU0,28u )
#define ADC0_LOGIC_CH29        CYHAL_GET_ADC( ADC_GROPU0,29u )
#define ADC0_LOGIC_CH30        CYHAL_GET_ADC( ADC_GROPU0,30u )
#define ADC0_LOGIC_CH31        CYHAL_GET_ADC( ADC_GROPU0,31u )

/** ADC[1] */
#define ADC1_LOGIC_CH0         CYHAL_GET_ADC( ADC_GROPU1,0u ) /*!< ADC[1] logic channel 0*/
#define ADC1_LOGIC_CH1         CYHAL_GET_ADC( ADC_GROPU1,1u ) 
#define ADC1_LOGIC_CH2         CYHAL_GET_ADC( ADC_GROPU1,2u ) 
#define ADC1_LOGIC_CH3         CYHAL_GET_ADC( ADC_GROPU1,3u ) 
#define ADC1_LOGIC_CH4         CYHAL_GET_ADC( ADC_GROPU1,4u ) 
#define ADC1_LOGIC_CH5         CYHAL_GET_ADC( ADC_GROPU1,5u ) 
#define ADC1_LOGIC_CH6         CYHAL_GET_ADC( ADC_GROPU1,6u ) 
#define ADC1_LOGIC_CH7         CYHAL_GET_ADC( ADC_GROPU1,7u ) 
#define ADC1_LOGIC_CH8         CYHAL_GET_ADC( ADC_GROPU1,8u ) 
#define ADC1_LOGIC_CH9         CYHAL_GET_ADC( ADC_GROPU1,9u ) 
#define ADC1_LOGIC_CH10        CYHAL_GET_ADC( ADC_GROPU1,10u )
#define ADC1_LOGIC_CH11        CYHAL_GET_ADC( ADC_GROPU1,11u )
#define ADC1_LOGIC_CH12        CYHAL_GET_ADC( ADC_GROPU1,12u )
#define ADC1_LOGIC_CH13        CYHAL_GET_ADC( ADC_GROPU1,13u )
#define ADC1_LOGIC_CH14        CYHAL_GET_ADC( ADC_GROPU1,14u )
#define ADC1_LOGIC_CH15        CYHAL_GET_ADC( ADC_GROPU1,15u )
#define ADC1_LOGIC_CH16        CYHAL_GET_ADC( ADC_GROPU1,16u )
#define ADC1_LOGIC_CH17        CYHAL_GET_ADC( ADC_GROPU1,17u )
#define ADC1_LOGIC_CH18        CYHAL_GET_ADC( ADC_GROPU1,18u )
#define ADC1_LOGIC_CH19        CYHAL_GET_ADC( ADC_GROPU1,19u )
#define ADC1_LOGIC_CH20        CYHAL_GET_ADC( ADC_GROPU1,20u )
#define ADC1_LOGIC_CH21        CYHAL_GET_ADC( ADC_GROPU1,21u )
#define ADC1_LOGIC_CH22        CYHAL_GET_ADC( ADC_GROPU1,22u )
#define ADC1_LOGIC_CH23        CYHAL_GET_ADC( ADC_GROPU1,23u )
#define ADC1_LOGIC_CH24        CYHAL_GET_ADC( ADC_GROPU1,24u )
#define ADC1_LOGIC_CH25        CYHAL_GET_ADC( ADC_GROPU1,25u )
#define ADC1_LOGIC_CH26        CYHAL_GET_ADC( ADC_GROPU1,26u )
#define ADC1_LOGIC_CH27        CYHAL_GET_ADC( ADC_GROPU1,27u )
#define ADC1_LOGIC_CH28        CYHAL_GET_ADC( ADC_GROPU1,28u )
#define ADC1_LOGIC_CH29        CYHAL_GET_ADC( ADC_GROPU1,29u )
#define ADC1_LOGIC_CH30        CYHAL_GET_ADC( ADC_GROPU1,30u )
#define ADC1_LOGIC_CH31        CYHAL_GET_ADC( ADC_GROPU1,31u )

/** ADC[2] */
#define ADC2_LOGIC_CH0         CYHAL_GET_ADC( ADC_GROPU2,0u ) /*!< ADC[2] logic channel 0*/
#define ADC2_LOGIC_CH1         CYHAL_GET_ADC( ADC_GROPU2,1u ) 
#define ADC2_LOGIC_CH2         CYHAL_GET_ADC( ADC_GROPU2,2u ) 
#define ADC2_LOGIC_CH3         CYHAL_GET_ADC( ADC_GROPU2,3u ) 
#define ADC2_LOGIC_CH4         CYHAL_GET_ADC( ADC_GROPU2,4u ) 
#define ADC2_LOGIC_CH5         CYHAL_GET_ADC( ADC_GROPU2,5u ) 
#define ADC2_LOGIC_CH6         CYHAL_GET_ADC( ADC_GROPU2,6u ) 
#define ADC2_LOGIC_CH7         CYHAL_GET_ADC( ADC_GROPU2,7u ) 
#define ADC2_LOGIC_CH8         CYHAL_GET_ADC( ADC_GROPU2,8u ) 
#define ADC2_LOGIC_CH9         CYHAL_GET_ADC( ADC_GROPU2,9u ) 
#define ADC2_LOGIC_CH10        CYHAL_GET_ADC( ADC_GROPU2,10u )
#define ADC2_LOGIC_CH11        CYHAL_GET_ADC( ADC_GROPU2,11u )
#define ADC2_LOGIC_CH12        CYHAL_GET_ADC( ADC_GROPU2,12u )
#define ADC2_LOGIC_CH13        CYHAL_GET_ADC( ADC_GROPU2,13u )
#define ADC2_LOGIC_CH14        CYHAL_GET_ADC( ADC_GROPU2,14u )
#define ADC2_LOGIC_CH15        CYHAL_GET_ADC( ADC_GROPU2,15u )
#define ADC2_LOGIC_CH16        CYHAL_GET_ADC( ADC_GROPU2,16u )
#define ADC2_LOGIC_CH17        CYHAL_GET_ADC( ADC_GROPU2,17u )
#define ADC2_LOGIC_CH18        CYHAL_GET_ADC( ADC_GROPU2,18u )
#define ADC2_LOGIC_CH19        CYHAL_GET_ADC( ADC_GROPU2,19u )
#define ADC2_LOGIC_CH20        CYHAL_GET_ADC( ADC_GROPU2,20u )
#define ADC2_LOGIC_CH21        CYHAL_GET_ADC( ADC_GROPU2,21u )
#define ADC2_LOGIC_CH22        CYHAL_GET_ADC( ADC_GROPU2,22u )
#define ADC2_LOGIC_CH23        CYHAL_GET_ADC( ADC_GROPU2,23u )
#define ADC2_LOGIC_CH24        CYHAL_GET_ADC( ADC_GROPU2,24u )
#define ADC2_LOGIC_CH25        CYHAL_GET_ADC( ADC_GROPU2,25u )
#define ADC2_LOGIC_CH26        CYHAL_GET_ADC( ADC_GROPU2,26u )
#define ADC2_LOGIC_CH27        CYHAL_GET_ADC( ADC_GROPU2,27u )
#define ADC2_LOGIC_CH28        CYHAL_GET_ADC( ADC_GROPU2,28u )
#define ADC2_LOGIC_CH29        CYHAL_GET_ADC( ADC_GROPU2,29u )
#define ADC2_LOGIC_CH30        CYHAL_GET_ADC( ADC_GROPU2,30u )
#define ADC2_LOGIC_CH31        CYHAL_GET_ADC( ADC_GROPU2,31u )

 
/*ADC Group configuration*/
extern adc_group_config_t gtdADC_Config;

/*ADC Channel configuration*/
extern adc_channel_config_t gtdADC_ChannelConfig;

#endif

  