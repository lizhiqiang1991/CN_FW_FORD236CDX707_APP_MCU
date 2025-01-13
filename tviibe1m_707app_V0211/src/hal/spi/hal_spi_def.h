/***************************************************************************
*\file hal_spi_def.h 
****************************************************************************/

#ifndef HAL_SPI_DEF_H
#define HAL_SPI_DEF_H

//cy_stc_scb_spi_config_t
/*SPI Configuration*/
typedef struct
{
    uint8_t u8MISO_pin;       /**< SPI MISO pin*/
    uint8_t u8MOSI_pin;       /**< SPI MOSI pin*/
    uint8_t u8CLK_Pin;        /**< SPI CLK pin*/
    uint8_t u8CS_Pin;         /**< SPI CS pin*/
    
    uint32_t u32Mode;           /**< SPI master/ slave*/
    uint32_t u32SubMode;        /**< Specifies the sub mode of SPI operation    ***/
    uint32_t u32SclkMode;       /**< Clock is active low, data is changed on first edge ***/
    uint32_t u32DataWidth;      /**< The width of RX/TX data (valid range 4-16).*/
    uint8_t  u8EnableMsbFirst;  /*** Enables the hardware to shift out the data element MSB first, otherwise, LSB first ***/
    
    uint32_t u32BaudRate;       /** Please set baudrate value of SPI you want */
    
}spi_config_t;

/* u32Mode */
#define SPI_MASTER   CY_SCB_SPI_MASTER
#define SPI_SLAVE    CY_SCB_SPI_SLAVE

/*u32SubMode*/
#define SPI_MOTOROLA      CY_SCB_SPI_MOTOROLA
#define SPI_TI_COINCIDES  CY_SCB_SPI_TI_COINCIDES
#define SPI_TI_PRECEDES   CY_SCB_SPI_TI_PRECEDES
#define SPI_NATIONAL      CY_SCB_SPI_NATIONAL

/*u32SclkMode*/
#define SPI_CPHA0_CPOL0 CY_SCB_SPI_CPHA0_CPOL0  /**< Clock is active low, data is changed on first edge   */
#define SPI_CPHA0_CPOL1 CY_SCB_SPI_CPHA0_CPOL1  /**< Clock is active high, data is changed on first edge  */
#define SPI_CPHA1_CPOL0 CY_SCB_SPI_CPHA1_CPOL0  /**< Clock is active low, data is changed on second edge  */
#define SPI_CPHA1_CPOL1 CY_SCB_SPI_CPHA1_CPOL1  /**< Clock is active high, data is changed on second edge */

/*u8EnableMsbFirst*/
#define SPI_MSB_FIRST     (1U)
#define SPI_LSB_FIRST     (0U)

/*Redefine SCB GROUP because I2C, SPI, or UART are the same hardware unit*/
#define SPI_GROPU0  CYHAL_SCB0
#define SPI_GROPU1  CYHAL_SCB1
#define SPI_GROPU2  CYHAL_SCB2
#define SPI_GROPU3  CYHAL_SCB3
#define SPI_GROPU4  CYHAL_SCB4
#define SPI_GROPU5  CYHAL_SCB5
#define SPI_GROPU6  CYHAL_SCB6
#define SPI_GROPU7  CYHAL_SCB7

#endif


  