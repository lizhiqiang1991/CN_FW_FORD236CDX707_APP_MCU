/***************************************************************************
*\file hal_spi.h 
****************************************************************************/

#ifndef HAL_SPI_H
#define HAL_SPI_H

uint8_t HAL_SPI_Init    (uint8_t u8SPIGroup, spi_config_t* tsConfig);
uint8_t HAL_SPI_DeInit  (uint8_t u8SPIGroup);
uint8_t HAL_SPI_Active  (uint8_t u8SPIGroup,uint8_t u8Enable);
/*
 If TVII is master mode, TVII transmits the data when "Cy_SCB_SPI_Transfer"
    Called. If TVII is slave, TVII transmits the data as responses when 
    master send some data to the TVII. 
*/
uint8_t HAL_SPI_Transfer(uint8_t u8SPIGroup,uint8_t* pTxBuff, uint8_t* pRxBuff,  uint32_t u32Size);
uint32_t HAL_SPI_TransferStatus_Get(uint8_t u8SPIGroup);

#endif

  